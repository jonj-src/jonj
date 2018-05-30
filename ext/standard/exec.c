/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The JONJ Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@jonj.tk>                              |
   |         Ilia Alshanetsky <iliaa@jonj.tk>                             |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include "jonj.h"
#include <ctype.h>
#include "jonj_string.h"
#include "ext/standard/head.h"
#include "ext/standard/file.h"
#include "basic_functions.h"
#include "exec.h"
#include "jonj_globals.h"
#include "SAPI.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef JONJ_WIN32
# include "win32/jonj_stdint.h"
#else
# if HAVE_INTTYPES_H
#  include <inttypes.h>
# elif HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

static int cmd_max_len;

/* {{{ JONJ_MINIT_FUNCTION(exec) */
JONJ_MINIT_FUNCTION(exec)
{
#ifdef _SC_ARG_MAX
	cmd_max_len = sysconf(_SC_ARG_MAX);
	if (-1 == cmd_max_len) {
#ifdef _POSIX_ARG_MAX
               cmd_max_len = _POSIX_ARG_MAX;
#else
               cmd_max_len = 4096;
#endif
       }
#elif defined(ARG_MAX)
	cmd_max_len = ARG_MAX;
#elif defined(JONJ_WIN32)
	/* Executed commands will run through cmd.exe. As long as it's the case,
		it's just the constant limit.*/
	cmd_max_len = 8192;
#else
	/* This is just an arbitrary value for the fallback case. */
	cmd_max_len = 4096;
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ jonj_exec
 * If type==0, only last line of output is returned (exec)
 * If type==1, all lines will be printed and last lined returned (system)
 * If type==2, all lines will be saved to given array (exec with &$array)
 * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
 *
 */
JONJAPI int jonj_exec(int type, char *cmd, zval *array, zval *return_value TSRMLS_DC)
{
	FILE *fp;
	char *buf;
	int l = 0, pclose_return;
	char *b, *d=NULL;
	jonj_stream *stream;
	size_t buflen, bufl = 0;
#if JONJ_SIGCHILD
	void (*sig_handler)() = NULL;
#endif

#if JONJ_SIGCHILD
	sig_handler = signal (SIGCHLD, SIG_DFL);
#endif

#ifdef JONJ_WIN32
	fp = VCWD_POPEN(cmd, "rb");
#else
	fp = VCWD_POPEN(cmd, "r");
#endif
	if (!fp) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", cmd);
		goto err;
	}

	stream = jonj_stream_fopen_from_pipe(fp, "rb");

	buf = (char *) emalloc(EXEC_INPUT_BUF);
	buflen = EXEC_INPUT_BUF;

	if (type != 3) {
		b = buf;

		while (jonj_stream_get_line(stream, b, EXEC_INPUT_BUF, &bufl)) {
			/* no new line found, let's read some more */
			if (b[bufl - 1] != '\n' && !jonj_stream_eof(stream)) {
				if (buflen < (bufl + (b - buf) + EXEC_INPUT_BUF)) {
					bufl += b - buf;
					buflen = bufl + EXEC_INPUT_BUF;
					buf = erealloc(buf, buflen);
					b = buf + bufl;
				} else {
					b += bufl;
				}
				continue;
			} else if (b != buf) {
				bufl += b - buf;
			}

			if (type == 1) {
				JONJWRITE(buf, bufl);
				if (jonj_output_get_level(TSRMLS_C) < 1) {
					sapi_flush(TSRMLS_C);
				}
			} else if (type == 2) {
				/* strip trailing whitespaces */
				l = bufl;
				while (l-- && isspace(((unsigned char *)buf)[l]));
				if (l != (int)(bufl - 1)) {
					bufl = l + 1;
					buf[bufl] = '\0';
				}
				add_next_index_stringl(array, buf, bufl, 1);
			}
			b = buf;
		}
		if (bufl) {
			/* strip trailing whitespaces if we have not done so already */
			if ((type == 2 && buf != b) || type != 2) {
				l = bufl;
				while (l-- && isspace(((unsigned char *)buf)[l]));
				if (l != (int)(bufl - 1)) {
					bufl = l + 1;
					buf[bufl] = '\0';
				}
				if (type == 2) {
					add_next_index_stringl(array, buf, bufl, 1);
				}
			}

			/* Return last line from the shell command */
			RETVAL_STRINGL(buf, bufl, 1);
		} else { /* should return NULL, but for BC we return "" */
			RETVAL_EMPTY_STRING();
		}
	} else {
		while((bufl = jonj_stream_read(stream, buf, EXEC_INPUT_BUF)) > 0) {
			JONJWRITE(buf, bufl);
		}
	}

	pclose_return = jonj_stream_close(stream);
	efree(buf);

done:
#if JONJ_SIGCHILD
	if (sig_handler) {
		signal(SIGCHLD, sig_handler);
	}
#endif
	if (d) {
		efree(d);
	}
	return pclose_return;
err:
	pclose_return = -1;
	goto done;
}
/* }}} */

static void jonj_exec_ex(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	char *cmd;
	int cmd_len;
	zval *ret_code=NULL, *ret_array=NULL;
	int ret;

	if (mode) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z/", &cmd, &cmd_len, &ret_code) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z/z/", &cmd, &cmd_len, &ret_array, &ret_code) == FAILURE) {
			RETURN_FALSE;
		}
	}
	if (!cmd_len) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command");
		RETURN_FALSE;
	}
	if (strlen(cmd) != cmd_len) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "NULL byte detected. Possible attack");
		RETURN_FALSE;
	}

	if (!ret_array) {
		ret = jonj_exec(mode, cmd, NULL, return_value TSRMLS_CC);
	} else {
		if (Z_TYPE_P(ret_array) != IS_ARRAY) {
			zval_dtor(ret_array);
			array_init(ret_array);
		}
		ret = jonj_exec(2, cmd, ret_array, return_value TSRMLS_CC);
	}
	if (ret_code) {
		zval_dtor(ret_code);
		ZVAL_LONG(ret_code, ret);
	}
}
/* }}} */

/* {{{ proto string exec(string command [, array &output [, int &return_value]])
   Execute an external program */
JONJ_FUNCTION(exec)
{
	jonj_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int system(string command [, int &return_value])
   Execute an external program and display output */
JONJ_FUNCTION(system)
{
	jonj_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void passthru(string command [, int &return_value])
   Execute an external program and display raw output */
JONJ_FUNCTION(passthru)
{
	jonj_exec_ex(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ jonj_escape_shell_cmd
   Escape all chars that could possibly be used to
   break out of a shell command

   This function emalloc's a string and returns the pointer.
   Remember to efree it when done with it.

   *NOT* safe for binary strings
*/
JONJAPI char *jonj_escape_shell_cmd(char *str)
{
	register int x, y;
	size_t l = strlen(str);
	uint64_t estimate = (2 * (uint64_t)l) + 1;
	char *cmd;
	char *p = NULL;

	TSRMLS_FETCH();

	/* max command line length - two single quotes - \0 byte length */
	if (l > cmd_max_len - 2 - 1) {
		jonj_error_docref(NULL TSRMLS_CC, E_ERROR, "Command exceeds the allowed length of %d bytes", cmd_max_len);
		return NULL;
	}

	cmd = safe_emalloc(2, l, 1);

	for (x = 0, y = 0; x < l; x++) {
		int mb_len = jonj_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(cmd + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
#ifndef JONJ_WIN32
			case '"':
			case '\'':
				if (!p && (p = memchr(str + x + 1, str[x], l - x - 1))) {
					/* noop */
				} else if (p && *p == str[x]) {
					p = NULL;
				} else {
					cmd[y++] = '\\';
				}
				cmd[y++] = str[x];
				break;
#else
			/* % is Windows specific for enviromental variables, ^%PATH% will
				output PATH while ^%PATH^% will not. escapeshellcmd will escape all % and !.
			*/
			case '%':
			case '!':
			case '"':
			case '\'':
#endif
			case '#': /* This is character-set independent */
			case '&':
			case ';':
			case '`':
			case '|':
			case '*':
			case '?':
			case '~':
			case '<':
			case '>':
			case '^':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '$':
			case '\\':
			case '\x0A': /* excluding these two */
			case '\xFF':
#ifdef JONJ_WIN32
				cmd[y++] = '^';
#else
				cmd[y++] = '\\';
#endif
				/* fall-through */
			default:
				cmd[y++] = str[x];

		}
	}
	cmd[y] = '\0';

	if (y > cmd_max_len + 1) {
		jonj_error_docref(NULL TSRMLS_CC, E_ERROR, "Escaped command exceeds the allowed length of %d bytes", cmd_max_len);
		efree(cmd);
		return NULL;
	}

	if ((estimate - y) > 4096) {
		/* realloc if the estimate was way overill
		 * Arbitrary cutoff point of 4096 */
		cmd = erealloc(cmd, y + 1);
	}

	return cmd;
}
/* }}} */

/* {{{ jonj_escape_shell_arg
 */
JONJAPI char *jonj_escape_shell_arg(char *str)
{
	int x, y = 0;
	size_t l = strlen(str);
	char *cmd;
	uint64_t estimate = (4 * (uint64_t)l) + 3;

	TSRMLS_FETCH();

	/* max command line length - two single quotes - \0 byte length */
	if (l > cmd_max_len - 2 - 1) {
		jonj_error_docref(NULL TSRMLS_CC, E_ERROR, "Argument exceeds the allowed length of %d bytes", cmd_max_len);
		return NULL;
	}

	cmd = safe_emalloc(4, l, 3); /* worst case */

#ifdef JONJ_WIN32
	cmd[y++] = '"';
#else
	cmd[y++] = '\'';
#endif

	for (x = 0; x < l; x++) {
		int mb_len = jonj_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(cmd + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
#ifdef JONJ_WIN32
		case '"':
		case '%':
		case '!':
			cmd[y++] = ' ';
			break;
#else
		case '\'':
			cmd[y++] = '\'';
			cmd[y++] = '\\';
			cmd[y++] = '\'';
#endif
			/* fall-through */
		default:
			cmd[y++] = str[x];
		}
	}
#ifdef JONJ_WIN32
	if (y > 0 && '\\' == cmd[y - 1]) {
		int k = 0, n = y - 1;
		for (; n >= 0 && '\\' == cmd[n]; n--, k++);
		if (k % 2) {
			cmd[y++] = '\\';
		}
	}

	cmd[y++] = '"';
#else
	cmd[y++] = '\'';
#endif
	cmd[y] = '\0';

	if (y > cmd_max_len + 1) {
		jonj_error_docref(NULL TSRMLS_CC, E_ERROR, "Escaped argument exceeds the allowed length of %d bytes", cmd_max_len);
		efree(cmd);
		return NULL;
	}

	if ((estimate - y) > 4096) {
		/* realloc if the estimate was way overill
		 * Arbitrary cutoff point of 4096 */
		cmd = erealloc(cmd, y + 1);
	}
	return cmd;
}
/* }}} */

/* {{{ proto string escapeshellcmd(string command)
   Escape shell metacharacters */
JONJ_FUNCTION(escapeshellcmd)
{
	char *command;
	int command_len;
	char *cmd = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command, &command_len) == FAILURE) {
		return;
	}

	if (command_len) {
		if (command_len != strlen(command)) {
			jonj_error_docref(NULL TSRMLS_CC, E_ERROR, "Input string contains NULL bytes");
			return;
		}
		cmd = jonj_escape_shell_cmd(command);
		RETVAL_STRINGL_CHECK(cmd, strlen(cmd), 0);
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string escapeshellarg(string arg)
   Quote and escape an argument for use in a shell command */
JONJ_FUNCTION(escapeshellarg)
{
	char *argument;
	int argument_len;
	char *cmd = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argument, &argument_len) == FAILURE) {
		return;
	}

	if (argument) {
		if (argument_len != strlen(argument)) {
			jonj_error_docref(NULL TSRMLS_CC, E_ERROR, "Input string contains NULL bytes");
			return;
		}
		cmd = jonj_escape_shell_arg(argument);
		RETVAL_STRINGL_CHECK(cmd, strlen(cmd), 0);
	}
}
/* }}} */

/* {{{ proto string shell_exec(string cmd)
   Execute command via shell and return complete output as string */
JONJ_FUNCTION(shell_exec)
{
	FILE *in;
	size_t total_readbytes;
	char *command;
	int command_len;
	char *ret;
	jonj_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command, &command_len) == FAILURE) {
		return;
	}

#ifdef JONJ_WIN32
	if ((in=VCWD_POPEN(command, "rt"))==NULL) {
#else
	if ((in=VCWD_POPEN(command, "r"))==NULL) {
#endif
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to execute '%s'", command);
		RETURN_FALSE;
	}

	stream = jonj_stream_fopen_from_pipe(in, "rb");
	total_readbytes = jonj_stream_copy_to_mem(stream, &ret, JONJ_STREAM_COPY_ALL, 0);
	jonj_stream_close(stream);

	if (total_readbytes > 0) {
		RETVAL_STRINGL_CHECK(ret, total_readbytes, 0);
	}
}
/* }}} */

#ifdef HAVE_NICE
/* {{{ proto bool proc_nice(int priority)
   Change the priority of the current process */
JONJ_FUNCTION(proc_nice)
{
	long pri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pri) == FAILURE) {
		RETURN_FALSE;
	}

	errno = 0;
	jonj_ignore_value(nice(pri));
	if (errno) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Only a super user may attempt to increase the priority of a process");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
