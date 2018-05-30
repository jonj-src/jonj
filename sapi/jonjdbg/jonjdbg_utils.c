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
   | Authors: Felipe Pena <felipe@jonj.tk>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@jonj.tk>                                |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "zend.h"
#include "jonj.h"
#include "spprintf.h"
#include "jonjdbg.h"
#include "jonjdbg_opcode.h"
#include "jonjdbg_utils.h"

#ifdef _WIN32
#	include "win32/time.h"
#elif defined(HAVE_SYS_IOCTL_H) 
#	include "sys/ioctl.h"
#	ifndef GWINSZ_IN_SYS_IOCTL
#		include <termios.h>
#	endif
#endif

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

/* {{{ color structures */
const static jonjdbg_color_t colors[] = {
	JONJDBG_COLOR_D("none",             "0;0"),

	JONJDBG_COLOR_D("white",            "0;64"),
	JONJDBG_COLOR_D("white-bold",       "1;64"),
	JONJDBG_COLOR_D("white-underline",  "4;64"),
	JONJDBG_COLOR_D("red",              "0;31"),
	JONJDBG_COLOR_D("red-bold",         "1;31"),
	JONJDBG_COLOR_D("red-underline",    "4;31"),
	JONJDBG_COLOR_D("green",            "0;32"),
	JONJDBG_COLOR_D("green-bold",       "1;32"),
	JONJDBG_COLOR_D("green-underline",  "4;32"),
	JONJDBG_COLOR_D("yellow",           "0;33"),
	JONJDBG_COLOR_D("yellow-bold",      "1;33"),
	JONJDBG_COLOR_D("yellow-underline", "4;33"),
	JONJDBG_COLOR_D("blue",             "0;34"),
	JONJDBG_COLOR_D("blue-bold",        "1;34"),
	JONJDBG_COLOR_D("blue-underline",   "4;34"),
	JONJDBG_COLOR_D("purple",           "0;35"),
	JONJDBG_COLOR_D("purple-bold",      "1;35"),
	JONJDBG_COLOR_D("purple-underline", "4;35"),
	JONJDBG_COLOR_D("cyan",             "0;36"),
	JONJDBG_COLOR_D("cyan-bold",        "1;36"),
	JONJDBG_COLOR_D("cyan-underline",   "4;36"),
	JONJDBG_COLOR_D("black",            "0;30"),
	JONJDBG_COLOR_D("black-bold",       "1;30"),
	JONJDBG_COLOR_D("black-underline",  "4;30"),
	JONJDBG_COLOR_END
}; /* }}} */

/* {{{ */
const static jonjdbg_element_t elements[] = {
	JONJDBG_ELEMENT_D("prompt", JONJDBG_COLOR_PROMPT),
	JONJDBG_ELEMENT_D("error", JONJDBG_COLOR_ERROR),
	JONJDBG_ELEMENT_D("notice", JONJDBG_COLOR_NOTICE),
	JONJDBG_ELEMENT_END
}; /* }}} */

JONJDBG_API int jonjdbg_is_numeric(const char *str) /* {{{ */
{
	if (!str)
		return 0;

	for (; *str; str++) {
		if (isspace(*str) || *str == '-') {
			continue;
		}
		return isdigit(*str);
	}
	return 0;
} /* }}} */

JONJDBG_API int jonjdbg_is_empty(const char *str) /* {{{ */
{
	if (!str)
		return 1;

	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return 0;
	}
	return 1;
} /* }}} */

JONJDBG_API int jonjdbg_is_addr(const char *str) /* {{{ */
{
	return str[0] && str[1] && memcmp(str, "0x", 2) == 0;
} /* }}} */

JONJDBG_API int jonjdbg_is_class_method(const char *str, size_t len, char **class, char **method) /* {{{ */
{
	char *sep = NULL;

	if (strstr(str, "#") != NULL)
		return 0;

	if (strstr(str, " ") != NULL)
		return 0;

	sep = strstr(str, "::");

	if (!sep || sep == str || sep+2 == str+len-1) {
		return 0;
	}

	if (class != NULL) {
	
		if (str[0] == '\\') {
			str++;
			len--;
		}
		
		*class = estrndup(str, sep - str);
		(*class)[sep - str] = 0;
	}

	if (method != NULL) {
		*method = estrndup(sep+2, str + len - (sep + 2));
	}

	return 1;
} /* }}} */

JONJDBG_API char *jonjdbg_resolve_path(const char *path TSRMLS_DC) /* {{{ */
{
	char resolved_name[MAXPATHLEN];

	if (expand_filepath(path, resolved_name TSRMLS_CC) == NULL) {
		return NULL;
	}

	return estrdup(resolved_name);
} /* }}} */

JONJDBG_API const char *jonjdbg_current_file(TSRMLS_D) /* {{{ */
{
	const char *file = zend_get_executed_filename(TSRMLS_C);

	if (memcmp(file, "[no active file]", sizeof("[no active file]")) == 0) {
		return JONJDBG_G(exec);
	}

	return file;
} /* }}} */

JONJDBG_API const zend_function *jonjdbg_get_function(const char *fname, const char *cname TSRMLS_DC) /* {{{ */
{
	zend_function *func = NULL;
	size_t fname_len = strlen(fname);
	char *lcname = zend_str_tolower_dup(fname, fname_len);

	if (cname) {
		zend_class_entry **ce;
		size_t cname_len = strlen(cname);
		char *lc_cname = zend_str_tolower_dup(cname, cname_len);
		int ret = zend_lookup_class(lc_cname, cname_len, &ce TSRMLS_CC);

		efree(lc_cname);

		if (ret == SUCCESS) {
			zend_hash_find(&(*ce)->function_table, lcname, fname_len+1,
				(void**)&func);
		}
	} else {
		zend_hash_find(EG(function_table), lcname, fname_len+1,
			(void**)&func);
	}

	efree(lcname);
	return func;
} /* }}} */

JONJDBG_API char *jonjdbg_trim(const char *str, size_t len, size_t *new_len) /* {{{ */
{
	const char *p = str;
	char *new = NULL;

	while (p && isspace(*p)) {
		++p;
		--len;
	}

	while (*p && isspace(*(p + len -1))) {
		--len;
	}

	if (len == 0) {
		new = estrndup("", sizeof(""));
		*new_len = 0;
	} else {
		new = estrndup(p, len);
		*(new + len) = '\0';

		if (new_len) {
			*new_len = len;
		}
	}

	return new;

} /* }}} */

JONJDBG_API int jonjdbg_print(int type TSRMLS_DC, FILE *fp, const char *format, ...) /* {{{ */
{
	int rc = 0;
	char *buffer = NULL;
	va_list args;

	if (format != NULL && strlen(format) > 0L) {
		va_start(args, format);
		vspprintf(&buffer, 0, format, args);
		va_end(args);
	}

	/* TODO(anyone) colours */

	switch (type) {
		case P_ERROR:
			if (JONJDBG_G(flags) & JONJDBG_IS_COLOURED) {
				rc = fprintf(fp,
						"\033[%sm[%s]\033[0m\n",
						JONJDBG_G(colors)[JONJDBG_COLOR_ERROR]->code, buffer);
			} else {
				rc = fprintf(fp, "[%s]\n", buffer);
			}
		break;

		case P_NOTICE:
			if (JONJDBG_G(flags) & JONJDBG_IS_COLOURED) {
				rc = fprintf(fp,
						"\033[%sm[%s]\033[0m\n",
						JONJDBG_G(colors)[JONJDBG_COLOR_NOTICE]->code, buffer);
			} else {
				rc = fprintf(fp, "[%s]\n", buffer);
			}
		break;

		case P_WRITELN: {
			if (buffer) {
				rc = fprintf(fp, "%s\n", buffer);
			} else {
				rc = fprintf(fp, "\n");
			}
		} break;

		case P_WRITE:
			if (buffer) {
				rc = fprintf(fp, "%s", buffer);
			}
		break;

		/* no formatting on logging output */
		case P_LOG:
			if (buffer) {
				struct timeval tp;
				if (gettimeofday(&tp, NULL) == SUCCESS) {
					rc = fprintf(fp, "[%ld %.8F]: %s\n", tp.tv_sec, tp.tv_usec / 1000000.00, buffer);
				} else {
					rc = FAILURE;
				}
			}
			break;
	}

	if (buffer) {
		efree(buffer);
	}

	return rc;
} /* }}} */

JONJDBG_API int jonjdbg_rlog(FILE *fp, const char *fmt, ...) { /* {{{ */
	int rc = 0;

	va_list args;
	struct timeval tp;

	va_start(args, fmt);
	if (gettimeofday(&tp, NULL) == SUCCESS) {
		char friendly[100];
		char *format = NULL, *buffer = NULL;
		const time_t tt = tp.tv_sec;

		strftime(friendly, 100, "%a %b %d %T.%%04d %Y", localtime(&tt));
		asprintf(
			&buffer, friendly, tp.tv_usec/1000);
		asprintf(
			&format, "[%s]: %s\n", buffer, fmt);
		rc = vfprintf(
			fp, format, args);

		free(format);
		free(buffer);
	}
	va_end(args);

	return rc;
} /* }}} */

JONJDBG_API const jonjdbg_color_t *jonjdbg_get_color(const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	const jonjdbg_color_t *color = colors;

	while (color && color->name) {
		if (name_length == color->name_length &&
			memcmp(name, color->name, name_length) == SUCCESS) {
			jonjdbg_debug(
				"jonjdbg_get_color(%s, %lu): %s", name, name_length, color->code);
			return color;
		}
		++color;
	}

	jonjdbg_debug(
		"jonjdbg_get_color(%s, %lu): failed", name, name_length);

	return NULL;
} /* }}} */

JONJDBG_API void jonjdbg_set_color(int element, const jonjdbg_color_t *color TSRMLS_DC) /* {{{ */
{
	JONJDBG_G(colors)[element] = color;
} /* }}} */

JONJDBG_API void jonjdbg_set_color_ex(int element, const char *name, size_t name_length TSRMLS_DC) /* {{{ */
{
	const jonjdbg_color_t *color = jonjdbg_get_color(name, name_length TSRMLS_CC);

	if (color) {
		jonjdbg_set_color(element, color TSRMLS_CC);
	} else JONJDBG_G(colors)[element] = colors;
} /* }}} */

JONJDBG_API const jonjdbg_color_t* jonjdbg_get_colors(TSRMLS_D) /* {{{ */
{
	return colors;
} /* }}} */

JONJDBG_API int jonjdbg_get_element(const char *name, size_t len TSRMLS_DC) {
	const jonjdbg_element_t *element = elements;
	
	while (element && element->name) {
		if (len == element->name_length) {
			if (strncasecmp(name, element->name, len) == SUCCESS) {
				return element->id;
			}
		}
		element++;
	}
	
	return JONJDBG_COLOR_INVALID;
}

JONJDBG_API void jonjdbg_set_prompt(const char *prompt TSRMLS_DC) /* {{{ */
{
	/* free formatted prompt */
	if (JONJDBG_G(prompt)[1]) {
		free(JONJDBG_G(prompt)[1]);
		JONJDBG_G(prompt)[1] = NULL;
	}
	/* free old prompt */
	if (JONJDBG_G(prompt)[0]) {
		free(JONJDBG_G(prompt)[0]);
		JONJDBG_G(prompt)[0] = NULL;
	}

	/* copy new prompt */
	JONJDBG_G(prompt)[0] = strdup(prompt);
} /* }}} */

JONJDBG_API const char *jonjdbg_get_prompt(TSRMLS_D) /* {{{ */
{
	/* find cached prompt */
	if (JONJDBG_G(prompt)[1]) {
		return JONJDBG_G(prompt)[1];
	}

	/* create cached prompt */
#ifndef HAVE_LIBEDIT
	/* TODO: libedit doesn't seems to support coloured prompt */
	if ((JONJDBG_G(flags) & JONJDBG_IS_COLOURED)) {
		asprintf(
			&JONJDBG_G(prompt)[1], "\033[%sm%s\033[0m ",
			JONJDBG_G(colors)[JONJDBG_COLOR_PROMPT]->code,
			JONJDBG_G(prompt)[0]);
	} else
#endif
	{
		asprintf(
			&JONJDBG_G(prompt)[1], "%s ",
			JONJDBG_G(prompt)[0]);
	}

	return JONJDBG_G(prompt)[1];
} /* }}} */

int jonjdbg_rebuild_symtable(TSRMLS_D) {
	if (!EG(active_op_array)) {
		jonjdbg_error("No active op array!");
		return FAILURE;
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);

		if (!EG(active_symbol_table)) {
			jonjdbg_error("No active symbol table!");
			return FAILURE;
		}
	}

	return SUCCESS;
}

JONJDBG_API int jonjdbg_get_terminal_width(TSRMLS_D) /* {{{ */
{
	int columns;	
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined(HAVE_SYS_IOCTL_H) && defined (TIOCGWINSZ)
	struct winsize w;

	columns = ioctl(fileno(stdout), TIOCGWINSZ, &w) == 0 ? w.ws_col : 80;
#else
	columns = 80;
#endif
	return columns;
} /* }}} */
