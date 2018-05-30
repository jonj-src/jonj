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

#if !defined(ZEND_SIGNALS) || defined(_WIN32)
# include <signal.h>
#endif
#include "jonjdbg.h"
#include "jonjdbg_prompt.h"
#include "jonjdbg_bp.h"
#include "jonjdbg_break.h"
#include "jonjdbg_list.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_set.h"
#include "zend_alloc.h"

/* {{{ remote console headers */
#ifndef _WIN32
#	include <sys/socket.h>
#	include <sys/select.h>
#	include <sys/time.h>
#	include <sys/types.h>
#	include <netinet/in.h>
#	include <unistd.h>
#	include <arpa/inet.h>
#endif /* }}} */

#if defined(JONJ_WIN32) && defined(HAVE_OPENSSL)
# include "openssl/applink.c"
#endif

ZEND_DECLARE_MODULE_GLOBALS(jonjdbg);

static zend_bool jonjdbg_booted = 0;

#if JONJ_VERSION_ID >= 50500
void (*zend_execute_old)(zend_execute_data *execute_data TSRMLS_DC);
#else
void (*zend_execute_old)(zend_op_array *op_array TSRMLS_DC);
#endif

static inline void jonj_jonjdbg_globals_ctor(zend_jonjdbg_globals *pg) /* {{{ */
{
	pg->prompt[0] = NULL;
	pg->prompt[1] = NULL;

	pg->colors[0] = NULL;
	pg->colors[1] = NULL;
	pg->colors[2] = NULL;

	pg->exec = NULL;
	pg->exec_len = 0;
	pg->buffer = NULL;
	pg->ops = NULL;
	pg->vmret = 0;
	pg->bp_count = 0;
	pg->flags = JONJDBG_DEFAULT_FLAGS;
	pg->oplog = NULL;
	pg->io[JONJDBG_STDIN] = NULL;
	pg->io[JONJDBG_STDOUT] = NULL;
	pg->io[JONJDBG_STDERR] = NULL;
	pg->frame.num = 0;
} /* }}} */

static JONJ_MINIT_FUNCTION(jonjdbg) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(jonjdbg, jonj_jonjdbg_globals_ctor, NULL);
#if JONJ_VERSION_ID >= 50500
	zend_execute_old = zend_execute_ex;
	zend_execute_ex = jonjdbg_execute_ex;
#else
	zend_execute_old = zend_execute;
	zend_execute = jonjdbg_execute_ex;
#endif

	REGISTER_STRINGL_CONSTANT("JONJDBG_VERSION", JONJDBG_VERSION, sizeof(JONJDBG_VERSION)-1, CONST_CS|CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("JONJDBG_FILE",   FILE_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JONJDBG_METHOD", METHOD_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JONJDBG_LINENO", NUMERIC_PARAM, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JONJDBG_FUNC",   STR_PARAM, CONST_CS|CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("JONJDBG_COLOR_PROMPT", JONJDBG_COLOR_PROMPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JONJDBG_COLOR_NOTICE", JONJDBG_COLOR_NOTICE, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JONJDBG_COLOR_ERROR",  JONJDBG_COLOR_ERROR, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
} /* }}} */

static void jonj_jonjdbg_destroy_bp_file(void *brake) /* {{{ */
{
	zend_hash_destroy((HashTable*)brake);
} /* }}} */

static void jonj_jonjdbg_destroy_bp_symbol(void *brake) /* {{{ */
{
	efree((char*)((jonjdbg_breaksymbol_t*)brake)->symbol);
} /* }}} */

static void jonj_jonjdbg_destroy_bp_opcode(void *brake) /* {{{ */
{
	efree((char*)((jonjdbg_breakop_t*)brake)->name);
} /* }}} */


static void jonj_jonjdbg_destroy_bp_methods(void *brake) /* {{{ */
{
	zend_hash_destroy((HashTable*)brake);
} /* }}} */

static void jonj_jonjdbg_destroy_bp_condition(void *data) /* {{{ */
{
	jonjdbg_breakcond_t *brake = (jonjdbg_breakcond_t*) data;

	if (brake) {
		if (brake->ops) {
			TSRMLS_FETCH();

			destroy_op_array(
					brake->ops TSRMLS_CC);
			efree(brake->ops);
		}
		efree((char*)brake->code);
	}
} /* }}} */

static void jonj_jonjdbg_destroy_registered(void *data) /* {{{ */
{
	zend_function *function = (zend_function*) data;
	TSRMLS_FETCH();

	destroy_zend_function(
		function TSRMLS_CC);
} /* }}} */


static JONJ_RINIT_FUNCTION(jonjdbg) /* {{{ */
{
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE],   8, NULL, jonj_jonjdbg_destroy_bp_file, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], 8, NULL, jonj_jonjdbg_destroy_bp_symbol, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE], 8, NULL, jonj_jonjdbg_destroy_bp_methods, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE], 8, NULL, jonj_jonjdbg_destroy_bp_methods, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE], 8, NULL, jonj_jonjdbg_destroy_bp_methods, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], 8, NULL, NULL, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE], 8, NULL, jonj_jonjdbg_destroy_bp_opcode, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD], 8, NULL, jonj_jonjdbg_destroy_bp_methods, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], 8, NULL, jonj_jonjdbg_destroy_bp_condition, 0);
	zend_hash_init(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], 8, NULL, NULL, 0);

	zend_hash_init(&JONJDBG_G(seek), 8, NULL, NULL, 0);
	zend_hash_init(&JONJDBG_G(registered), 8, NULL, jonj_jonjdbg_destroy_registered, 0);

	return SUCCESS;
} /* }}} */

static JONJ_RSHUTDOWN_FUNCTION(jonjdbg) /* {{{ */
{
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_COND]);
	zend_hash_destroy(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP]);
	zend_hash_destroy(&JONJDBG_G(seek));
	zend_hash_destroy(&JONJDBG_G(registered));
	zend_hash_destroy(&JONJDBG_G(watchpoints));
	zend_llist_destroy(&JONJDBG_G(watchlist_mem));

	if (JONJDBG_G(buffer)) {
		efree(JONJDBG_G(buffer));
		JONJDBG_G(buffer) = NULL;
	}
	
	if (JONJDBG_G(exec)) {
		efree(JONJDBG_G(exec));
		JONJDBG_G(exec) = NULL;
	}

	if (JONJDBG_G(prompt)[0]) {
		free(JONJDBG_G(prompt)[0]);
	}
	if (JONJDBG_G(prompt)[1]) {
		free(JONJDBG_G(prompt)[1]);
	}

	JONJDBG_G(prompt)[0] = NULL;
	JONJDBG_G(prompt)[1] = NULL;

	if (JONJDBG_G(oplog)) {
		fclose(
				JONJDBG_G(oplog));
		JONJDBG_G(oplog) = NULL;
	}

	if (JONJDBG_G(ops)) {
		destroy_op_array(JONJDBG_G(ops) TSRMLS_CC);
		efree(JONJDBG_G(ops));
		JONJDBG_G(ops) = NULL;
	}

	return SUCCESS;
} /* }}} */

/* {{{ proto mixed jonjdbg_exec(string context)
	Attempt to set the execution context for jonjdbg
	If the execution context was set previously it is returned
	If the execution context was not set previously boolean true is returned
	If the request to set the context fails, boolean false is returned, and an E_WARNING raised */
static JONJ_FUNCTION(jonjdbg_exec)
{
	char *exec = NULL;
	int exec_len = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &exec, &exec_len) == FAILURE) {
		return;
	}

	{
		struct stat sb;
		zend_bool result = 1;

		if (VCWD_STAT(exec, &sb) != FAILURE) {
			if (sb.st_mode & (S_IFREG|S_IFLNK)) {
				if (JONJDBG_G(exec)) {
					ZVAL_STRINGL(return_value, JONJDBG_G(exec), JONJDBG_G(exec_len), 1);
					efree(JONJDBG_G(exec));
					result = 0;
				}

				JONJDBG_G(exec) = estrndup(exec, exec_len);
				JONJDBG_G(exec_len) = exec_len;

				if (result)
					ZVAL_BOOL(return_value, 1);
			} else {
				zend_error(
					E_WARNING, "Failed to set execution context (%s), not a regular file or symlink", exec);
				ZVAL_BOOL(return_value, 0);
			}
		} else {
			zend_error(
				E_WARNING, "Failed to set execution context (%s) the file does not exist", exec);

			ZVAL_BOOL(return_value, 0);
		}
	}
} /* }}} */

/* {{{ proto void jonjdbg_break_next()
    instructs jonjdbg to insert a breakpoint at the next opcode */
static JONJ_FUNCTION(jonjdbg_break_next)
{
	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	} else if (EG(current_execute_data) && EG(active_op_array)) {
		zend_ulong opline_num = (EG(current_execute_data)->opline -
				EG(active_op_array)->opcodes);

		jonjdbg_set_breakpoint_opline_ex(
				&EG(active_op_array)->opcodes[opline_num+1] TSRMLS_CC);
	}
} /* }}} */

/* {{{ proto void jonjdbg_break_file(string file, integer line) */
static JONJ_FUNCTION(jonjdbg_break_file)
{
    char    *file = NULL;
    int      flen = 0;
    long     line;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &file, &flen, &line) == FAILURE) {
        return;
    }

    jonjdbg_set_breakpoint_file(file, line TSRMLS_CC);
} /* }}} */

/* {{{ proto void jonjdbg_break_method(string class, string method) */
static JONJ_FUNCTION(jonjdbg_break_method)
{
    char *class = NULL,
         *method = NULL;
    int clen = 0, 
        mlen = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &class, &clen, &method, &mlen) == FAILURE) {
    return;
    }

    jonjdbg_set_breakpoint_method(class, method TSRMLS_CC);
} /* }}} */

/* {{{ proto void jonjdbg_break_function(string function) */
static JONJ_FUNCTION(jonjdbg_break_function)
{
    char *function = NULL;
    int   function_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &function, &function_len) == FAILURE) {
           return;
    }

    jonjdbg_set_breakpoint_symbol(function, function_len TSRMLS_CC);
} /* }}} */

/* {{{ proto void jonjdbg_clear(void)
   instructs jonjdbg to clear breakpoints */
static JONJ_FUNCTION(jonjdbg_clear)
{
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_COND]);
} /* }}} */

/* {{{ proto void jonjdbg_color(integer element, string color) */
static JONJ_FUNCTION(jonjdbg_color)
{
	long element = 0L;
	char *color = NULL;
	int color_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &element, &color, &color_len) == FAILURE) {
		return;
	}

	switch (element) {
		case JONJDBG_COLOR_NOTICE:
		case JONJDBG_COLOR_ERROR:
		case JONJDBG_COLOR_PROMPT:
			jonjdbg_set_color_ex(element, color, color_len TSRMLS_CC);
		break;

		default: zend_error(E_ERROR, "jonjdbg detected an incorrect color constant");
	}
} /* }}} */

/* {{{ proto void jonjdbg_prompt(string prompt) */
static JONJ_FUNCTION(jonjdbg_prompt)
{
	char *prompt = NULL;
	int prompt_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &prompt, &prompt_len) == FAILURE) {
		return;
	}

	jonjdbg_set_prompt(prompt TSRMLS_CC);
} /* }}} */

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_break_next_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_break_file_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, file)
    ZEND_ARG_INFO(0, line)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_break_method_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, class)
    ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_break_function_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_color_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, element)
	ZEND_ARG_INFO(0, color)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_prompt_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_exec_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(jonjdbg_clear_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_function_entry jonjdbg_user_functions[] = {
	JONJ_FE(jonjdbg_clear, jonjdbg_clear_arginfo)
	JONJ_FE(jonjdbg_break_next, jonjdbg_break_next_arginfo)
	JONJ_FE(jonjdbg_break_file, jonjdbg_break_file_arginfo)
	JONJ_FE(jonjdbg_break_method, jonjdbg_break_method_arginfo)
	JONJ_FE(jonjdbg_break_function, jonjdbg_break_function_arginfo)
	JONJ_FE(jonjdbg_exec,  jonjdbg_exec_arginfo)
	JONJ_FE(jonjdbg_color, jonjdbg_color_arginfo)
	JONJ_FE(jonjdbg_prompt, jonjdbg_prompt_arginfo)
#ifdef  JONJ_FE_END
	JONJ_FE_END
#else
	{NULL,NULL,NULL}
#endif
};

static zend_module_entry sapi_jonjdbg_module_entry = {
	STANDARD_MODULE_HEADER,
	JONJDBG_NAME,
	jonjdbg_user_functions,
	JONJ_MINIT(jonjdbg),
	NULL,
	JONJ_RINIT(jonjdbg),
	JONJ_RSHUTDOWN(jonjdbg),
	NULL,
	JONJDBG_VERSION,
	STANDARD_MODULE_PROPERTIES
};

static inline int jonj_sapi_jonjdbg_module_startup(sapi_module_struct *module) /* {{{ */
{
	if (jonj_module_startup(module, &sapi_jonjdbg_module_entry, 1) == FAILURE) {
		return FAILURE;
	}

	jonjdbg_booted=1;

	return SUCCESS;
} /* }}} */

static char* jonj_sapi_jonjdbg_read_cookies(TSRMLS_D) /* {{{ */
{
	return NULL;
} /* }}} */

static int jonj_sapi_jonjdbg_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s TSRMLS_DC) /* {{{ */
{
	return 0;
}
/* }}} */

static int jonj_sapi_jonjdbg_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC) /* {{{ */
{
	/* We do nothing here, this function is needed to prevent that the fallback
	 * header handling is called. */
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static void jonj_sapi_jonjdbg_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC) /* {{{ */
{
}
/* }}} */

static void jonj_sapi_jonjdbg_log_message(char *message TSRMLS_DC) /* {{{ */
{
	/*
	* We must not request TSRM before being boot
	*/
	if (jonjdbg_booted) {
		jonjdbg_error("%s", message);

		switch (PG(last_error_type)) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
			case E_PARSE:
			case E_RECOVERABLE_ERROR:
				if (!(JONJDBG_G(flags) & JONJDBG_IN_EVAL)) {
					jonjdbg_list_file(
						zend_get_executed_filename(TSRMLS_C),
						3,
						zend_get_executed_lineno(TSRMLS_C)-1,
						zend_get_executed_lineno(TSRMLS_C)
						TSRMLS_CC
					);
				}

				do {
					switch (jonjdbg_interactive(TSRMLS_C)) {
						case JONJDBG_LEAVE:
						case JONJDBG_FINISH:
						case JONJDBG_UNTIL:
						case JONJDBG_NEXT:
							return;
					}
				} while (!(JONJDBG_G(flags) & JONJDBG_IS_QUITTING));

		}
	} else fprintf(stdout, "%s\n", message);
}
/* }}} */

static int jonj_sapi_jonjdbg_deactivate(TSRMLS_D) /* {{{ */
{
	fflush(stdout);
	if(SG(request_info).argv0) {
		free(SG(request_info).argv0);
		SG(request_info).argv0 = NULL;
	}
	return SUCCESS;
}
/* }}} */

static void jonj_sapi_jonjdbg_register_vars(zval *track_vars_array TSRMLS_DC) /* {{{ */
{
	unsigned int len;
	char   *docroot = "";

	/* In jonjdbg mode, we consider the environment to be a part of the server variables
	*/
	jonj_import_environment_variables(track_vars_array TSRMLS_CC);

	if (JONJDBG_G(exec)) {
		len = JONJDBG_G(exec_len);
		if (sapi_module.input_filter(PARSE_SERVER, "JONJ_SELF",
					&JONJDBG_G(exec), JONJDBG_G(exec_len), &len TSRMLS_CC)) {
			jonj_register_variable("JONJ_SELF", JONJDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}
		if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_NAME",
					&JONJDBG_G(exec), JONJDBG_G(exec_len), &len TSRMLS_CC)) {
			jonj_register_variable("SCRIPT_NAME", JONJDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}

		if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_FILENAME",
					&JONJDBG_G(exec), JONJDBG_G(exec_len), &len TSRMLS_CC)) {
			jonj_register_variable("SCRIPT_FILENAME", JONJDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}
		if (sapi_module.input_filter(PARSE_SERVER, "PATH_TRANSLATED",
					&JONJDBG_G(exec), JONJDBG_G(exec_len), &len TSRMLS_CC)) {
			jonj_register_variable("PATH_TRANSLATED", JONJDBG_G(exec),
					track_vars_array TSRMLS_CC);
		}
	}

	/* any old docroot will doo */
	len = 0U;
	if (sapi_module.input_filter(PARSE_SERVER, "DOCUMENT_ROOT",
				&docroot, len, &len TSRMLS_CC)) {
		jonj_register_variable("DOCUMENT_ROOT", docroot, track_vars_array TSRMLS_CC);
	}
}
/* }}} */

static inline int jonj_sapi_jonjdbg_ub_write(const char *message, unsigned int length TSRMLS_DC) /* {{{ */
{
	return jonjdbg_write("%s", message);
} /* }}} */

#if JONJ_VERSION_ID >= 50700
static inline void jonj_sapi_jonjdbg_flush(void *context TSRMLS_DC)  /* {{{ */
{
#else
static inline void jonj_sapi_jonjdbg_flush(void *context)  /* {{{ */
{
	TSRMLS_FETCH();
#endif

	fflush(JONJDBG_G(io)[JONJDBG_STDOUT]);
} /* }}} */

/* copied from sapi/cli/jonj_cli.c cli_register_file_handles */
static void jonjdbg_register_file_handles(TSRMLS_D) /* {{{ */
{
	zval *zin, *zout, *zerr;
	jonj_stream *s_in, *s_out, *s_err;
	jonj_stream_context *sc_in=NULL, *sc_out=NULL, *sc_err=NULL;
	zend_constant ic, oc, ec;

	MAKE_STD_ZVAL(zin);
	MAKE_STD_ZVAL(zout);
	MAKE_STD_ZVAL(zerr);

	s_in  = jonj_stream_open_wrapper_ex("jonj://stdin",  "rb", 0, NULL, sc_in);
	s_out = jonj_stream_open_wrapper_ex("jonj://stdout", "wb", 0, NULL, sc_out);
	s_err = jonj_stream_open_wrapper_ex("jonj://stderr", "wb", 0, NULL, sc_err);

	if (s_in==NULL || s_out==NULL || s_err==NULL) {
		FREE_ZVAL(zin);
		FREE_ZVAL(zout);
		FREE_ZVAL(zerr);
		if (s_in) jonj_stream_close(s_in);
		if (s_out) jonj_stream_close(s_out);
		if (s_err) jonj_stream_close(s_err);
		return;
	}

#if JONJ_DEBUG
	/* do not close stdout and stderr */
	s_out->flags |= JONJ_STREAM_FLAG_NO_CLOSE;
	s_err->flags |= JONJ_STREAM_FLAG_NO_CLOSE;
#endif

	jonj_stream_to_zval(s_in,  zin);
	jonj_stream_to_zval(s_out, zout);
	jonj_stream_to_zval(s_err, zerr);

	ic.value = *zin;
	ic.flags = CONST_CS;
	ic.name = zend_strndup(ZEND_STRL("STDIN"));
	ic.name_len = sizeof("STDIN");
	ic.module_number = 0;
	zend_register_constant(&ic TSRMLS_CC);

	oc.value = *zout;
	oc.flags = CONST_CS;
	oc.name = zend_strndup(ZEND_STRL("STDOUT"));
	oc.name_len = sizeof("STDOUT");
	oc.module_number = 0;
	zend_register_constant(&oc TSRMLS_CC);

	ec.value = *zerr;
	ec.flags = CONST_CS;
	ec.name = zend_strndup(ZEND_STRL("STDERR"));
	ec.name_len = sizeof("STDERR");
	ec.module_number = 0;
	zend_register_constant(&ec TSRMLS_CC);

	FREE_ZVAL(zin);
	FREE_ZVAL(zout);
	FREE_ZVAL(zerr);
}
/* }}} */

/* {{{ sapi_module_struct jonjdbg_sapi_module
*/
static sapi_module_struct jonjdbg_sapi_module = {
	"jonjdbg",                       /* name */
	"jonjdbg",                       /* pretty name */

	jonj_sapi_jonjdbg_module_startup, /* startup */
	jonj_module_shutdown_wrapper,    /* shutdown */

	NULL,                           /* activate */
	jonj_sapi_jonjdbg_deactivate,     /* deactivate */

	jonj_sapi_jonjdbg_ub_write,       /* unbuffered write */
	jonj_sapi_jonjdbg_flush,          /* flush */
	NULL,                           /* get uid */
	NULL,                           /* getenv */

	jonj_error,                      /* error handler */

	jonj_sapi_jonjdbg_header_handler, /* header handler */
	jonj_sapi_jonjdbg_send_headers,   /* send headers handler */
	jonj_sapi_jonjdbg_send_header,    /* send header handler */

	NULL,                           /* read POST data */
	jonj_sapi_jonjdbg_read_cookies,   /* read Cookies */

	jonj_sapi_jonjdbg_register_vars,  /* register server variables */
	jonj_sapi_jonjdbg_log_message,    /* Log message */
	NULL,                           /* Get request time */
	NULL,                           /* Child terminate */
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

const opt_struct OPTIONS[] = { /* {{{ */
	{'c', 1, "ini path override"},
	{'d', 1, "define ini entry on command line"},
	{'n', 0, "no jonj.ini"},
	{'z', 1, "load zend_extension"},
	/* jonjdbg options */
	{'q', 0, "no banner"},
	{'v', 0, "disable quietness"},
	{'s', 0, "enable stepping"},
	{'b', 0, "boring colours"},
	{'i', 1, "specify init"},
	{'I', 0, "ignore init"},
	{'O', 1, "opline log"},
	{'r', 0, "run"},
	{'E', 0, "step-through-eval"},
	{'S', 1, "sapi-name"},
#ifndef _WIN32
	{'l', 1, "listen"},
	{'a', 1, "address-or-any"},
#endif
	{'V', 0, "version"},
	{'-', 0, NULL}
}; /* }}} */

const char jonjdbg_ini_hardcoded[] =
"html_errors=Off\n"
"register_argc_argv=On\n"
"implicit_flush=On\n"
"display_errors=Off\n"
"log_errors=On\n"
"max_execution_time=0\n"
"max_input_time=-1\n"
"error_log=\n"
"output_buffering=off\0";

/* overwriteable ini defaults must be set in jonjdbg_ini_defaults() */
#define INI_DEFAULT(name, value) \
	Z_SET_REFCOUNT(tmp, 0); \
	Z_UNSET_ISREF(tmp); \
	ZVAL_STRINGL(&tmp, zend_strndup(value, sizeof(value)-1), sizeof(value)-1, 0); \
	zend_hash_update(configuration_hash, name, sizeof(name), &tmp, sizeof(zval), NULL);

void jonjdbg_ini_defaults(HashTable *configuration_hash) /* {{{ */
{
	zval tmp;
	INI_DEFAULT("report_zend_debug", "0");
} /* }}} */

static void jonjdbg_welcome(zend_bool cleaning TSRMLS_DC) /* {{{ */
{
	/* print blurb */
	if (!cleaning) {
		jonjdbg_notice("Welcome to jonjdbg, the interactive JONJ debugger, v%s",
				JONJDBG_VERSION);
		jonjdbg_writeln("To get help using jonjdbg type \"help\" and press enter");
		jonjdbg_notice("Please report bugs to <%s>", JONJDBG_ISSUES);
	} else {
		jonjdbg_notice("Clean Execution Environment");

		jonjdbg_writeln("Classes\t\t\t%d", zend_hash_num_elements(EG(class_table)));
		jonjdbg_writeln("Functions\t\t%d", zend_hash_num_elements(EG(function_table)));
		jonjdbg_writeln("Constants\t\t%d", zend_hash_num_elements(EG(zend_constants)));
		jonjdbg_writeln("Includes\t\t%d",  zend_hash_num_elements(&EG(included_files)));
	}
} /* }}} */

static inline void jonjdbg_sigint_handler(int signo) /* {{{ */
{
	TSRMLS_FETCH();

	if (EG(in_execution)) {
		/* set signalled only when not interactive */
		if (!(JONJDBG_G(flags) & JONJDBG_IS_INTERACTIVE)) {
			JONJDBG_G(flags) |= JONJDBG_IS_SIGNALED;
		}
	} else {
		/* we quit remote consoles on recv SIGINT */
		if (JONJDBG_G(flags) & JONJDBG_IS_REMOTE) {
			JONJDBG_G(flags) |= JONJDBG_IS_QUITTING;
			zend_bailout();
		}
	}
} /* }}} */

#ifndef _WIN32
int jonjdbg_open_socket(const char *interface, short port) /* {{{ */
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	switch (fd) {
		case -1:
			return -1;

		default: {
			int reuse = 1;

			switch (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse))) {
				case -1:
					close(fd);
					return -2;

				default: {
					struct sockaddr_in address;

					memset(&address, 0, sizeof(address));

					address.sin_port = htons(port);
					address.sin_family = AF_INET;

					if ((*interface == '*')) {
						address.sin_addr.s_addr = htonl(INADDR_ANY);
					} else if (!inet_pton(AF_INET, interface, &address.sin_addr)) {
						close(fd);
						return -3;
					}

					switch (bind(fd, (struct sockaddr *)&address, sizeof(address))) {
						case -1:
							close(fd);
							return -4;

						default: {
							listen(fd, 5);
						}
					}
				}
			}
		}
	}

	return fd;
} /* }}} */

static inline void jonjdbg_close_sockets(int (*socket)[2], FILE *streams[2]) /* {{{ */
{
	if ((*socket)[0] >= 0) {
		shutdown(
			(*socket)[0], SHUT_RDWR);
		close((*socket)[0]);
	}

	if (streams[0]) {
		fclose(streams[0]);
	}

	if ((*socket)[1] >= 0) {
		shutdown(
			(*socket)[1], SHUT_RDWR);
		close((*socket)[1]);
	}

	if (streams[1]) {
		fclose(streams[1]);
	}
} /* }}} */

/* don't inline this, want to debug it easily, will inline when done */

int jonjdbg_open_sockets(char *address, int port[2], int (*listen)[2], int (*socket)[2], FILE* streams[2]) /* {{{ */
{
	if (((*listen)[0]) < 0 && ((*listen)[1]) < 0) {
		((*listen)[0]) = jonjdbg_open_socket(address, (short)port[0]);
		((*listen)[1]) = jonjdbg_open_socket(address, (short)port[1]);
	}

	streams[0] = NULL;
	streams[1] = NULL;

	if ((*listen)[0] < 0 || (*listen)[1] < 0) {
		if ((*listen)[0] < 0) {
			jonjdbg_rlog(stderr,
				"console failed to initialize (stdin) on %s:%d", address, port[0]);
		}

		if ((*listen)[1] < 0) {
			jonjdbg_rlog(stderr,
				"console failed to initialize (stdout) on %s:%d", address, port[1]);
		}

		if ((*listen)[0] >= 0) {
			close((*listen)[0]);
		}

		if ((*listen)[1] >= 0) {
			close((*listen)[1]);
		}

		return FAILURE;
	}

	jonjdbg_close_sockets(socket, streams);

	jonjdbg_rlog(stderr,
		"accepting connections on %s:%d/%d", address, port[0], port[1]);
	{
		struct sockaddr_in address;
		socklen_t size = sizeof(address);
		char buffer[20] = {0};

		{
			memset(&address, 0, size);
			(*socket)[0] = accept(
				(*listen)[0], (struct sockaddr *) &address, &size);
			inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer));

			jonjdbg_rlog(stderr, "connection (stdin) from %s", buffer);
		}

		{
			memset(&address, 0, size);
			(*socket)[1] = accept(
				(*listen)[1], (struct sockaddr *) &address, &size);
		    inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer));

			jonjdbg_rlog(stderr, "connection (stdout) from %s", buffer);
		}
	}

	dup2((*socket)[0], fileno(stdin));
	dup2((*socket)[1], fileno(stdout));

	setbuf(stdout, NULL);

	streams[0] = fdopen((*socket)[0], "r");
	streams[1] = fdopen((*socket)[1], "w");

	return SUCCESS;
} /* }}} */

void jonjdbg_signal_handler(int sig, siginfo_t *info, void *context) /* {{{ */
{
	int is_handled = FAILURE;
	TSRMLS_FETCH();

	switch (sig) {
		case SIGBUS:
		case SIGSEGV:
			is_handled = jonjdbg_watchpoint_segfault_handler(info, context TSRMLS_CC);
			if (is_handled == FAILURE) {
#ifdef ZEND_SIGNALS
				zend_sigaction(sig, &JONJDBG_G(old_sigsegv_signal), NULL TSRMLS_CC);
#else
				sigaction(sig, &JONJDBG_G(old_sigsegv_signal), NULL);
#endif
			}
			break;
	}

} /* }}} */
#endif

static inline zend_mm_heap *jonjdbg_mm_get_heap() /* {{{ */
{
	zend_mm_heap *mm_heap;

	TSRMLS_FETCH();

	mm_heap = zend_mm_set_heap(NULL TSRMLS_CC);
	zend_mm_set_heap(mm_heap TSRMLS_CC);

	return mm_heap;
} /* }}} */

void *jonjdbg_malloc_wrapper(size_t size) /* {{{ */
{
	return zend_mm_alloc(jonjdbg_mm_get_heap(), size);
} /* }}} */

void jonjdbg_free_wrapper(void *p) /* {{{ */
{
	zend_mm_free(jonjdbg_mm_get_heap(), p);
} /* }}} */

void *jonjdbg_realloc_wrapper(void *ptr, size_t size) /* {{{ */
{
	return zend_mm_realloc(jonjdbg_mm_get_heap(), ptr, size);
} /* }}} */

int main(int argc, char **argv) /* {{{ */
{
	sapi_module_struct *jonjdbg = &jonjdbg_sapi_module;
	char *sapi_name;
	char *ini_entries;
	int   ini_entries_len;
	char **zend_extensions = NULL;
	zend_ulong zend_extensions_len = 0L;
	zend_bool ini_ignore;
	char *ini_override;
	char *exec;
	size_t exec_len;
	char *init_file;
	size_t init_file_len;
	zend_bool init_file_default;
	char *oplog_file;
	size_t oplog_file_len;
	zend_ulong flags;
	char *jonj_optarg;
	int jonj_optind, opt, show_banner = 1;
	long cleaning = 0;
	zend_bool remote = 0;
	int run = 0;
	int step = 0;

#ifdef _WIN32
	char *bp_tmp_file = NULL;
#else
	char bp_tmp_file[] = "/tmp/jonjdbg.XXXXXX";
#endif

#ifndef _WIN32
	char *address;
	int listen[2];
	int server[2];
	int socket[2];
	FILE* streams[2] = {NULL, NULL};
#endif

#ifdef ZTS
	void ***tsrm_ls;
#endif

#ifndef _WIN32
	struct sigaction signal_struct;
	signal_struct.sa_sigaction = jonjdbg_signal_handler;
	signal_struct.sa_flags = SA_SIGINFO | SA_NODEFER;

	address = strdup("127.0.0.1");
	socket[0] = -1;
	socket[1] = -1;
	listen[0] = -1;
	listen[1] = -1;
	server[0] = -1;
	server[1] = -1;
	streams[0] = NULL;
	streams[1] = NULL;
#endif

#ifdef JONJ_WIN32
	_fmode = _O_BINARY;                 /* sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);  /* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY); /* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY); /* make the stdio mode be binary */
#endif

#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);

	tsrm_ls = ts_resource(0);
#endif

jonjdbg_main:
	if (!cleaning) {
	
#ifdef _WIN32
		bp_tmp_file = malloc(L_tmpnam);

		if (bp_tmp_file) {
			if (!tmpnam(bp_tmp_file)) {
				free(bp_tmp_file);
				bp_tmp_file = NULL;
			}
		}

		if (!bp_tmp_file) {
			jonjdbg_error("Unable to create temporary file");
			return 1;
		}
#else
		if (!mkstemp(bp_tmp_file)) {
			memset(bp_tmp_file, 0, sizeof(bp_tmp_file));
		}
#endif

	}
	ini_entries = NULL;
	ini_entries_len = 0;
	ini_ignore = 0;
	ini_override = NULL;
	zend_extensions = NULL;
	zend_extensions_len = 0L;
	exec = NULL;
	exec_len = 0;
	init_file = NULL;
	init_file_len = 0;
	init_file_default = 1;
	oplog_file = NULL;
	oplog_file_len = 0;
	flags = JONJDBG_DEFAULT_FLAGS;
	jonj_optarg = NULL;
	jonj_optind = 1;
	opt = 0;
	run = 0;
	step = 0;
	sapi_name = NULL;

	while ((opt = jonj_getopt(argc, argv, OPTIONS, &jonj_optarg, &jonj_optind, 0, 2)) != -1) {
		switch (opt) {
			case 'r':
				run++;
				break;
			case 'n':
				ini_ignore = 1;
				break;
			case 'c':
				if (ini_override) {
					free(ini_override);
				}
				ini_override = strdup(jonj_optarg);
				break;
			case 'd': {
				int len = strlen(jonj_optarg);
				char *val;

				if ((val = strchr(jonj_optarg, '='))) {
				  val++;
				  if (!isalnum(*val) && *val != '"' && *val != '\'' && *val != '\0') {
					  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("\"\"\n\0"));
					  memcpy(ini_entries + ini_entries_len, jonj_optarg, (val - jonj_optarg));
					  ini_entries_len += (val - jonj_optarg);
					  memcpy(ini_entries + ini_entries_len, "\"", 1);
					  ini_entries_len++;
					  memcpy(ini_entries + ini_entries_len, val, len - (val - jonj_optarg));
					  ini_entries_len += len - (val - jonj_optarg);
					  memcpy(ini_entries + ini_entries_len, "\"\n\0", sizeof("\"\n\0"));
					  ini_entries_len += sizeof("\n\0\"") - 2;
				  } else {
					  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("\n\0"));
					  memcpy(ini_entries + ini_entries_len, jonj_optarg, len);
					  memcpy(ini_entries + ini_entries_len + len, "\n\0", sizeof("\n\0"));
					  ini_entries_len += len + sizeof("\n\0") - 2;
				  }
				} else {
				  ini_entries = realloc(ini_entries, ini_entries_len + len + sizeof("=1\n\0"));
				  memcpy(ini_entries + ini_entries_len, jonj_optarg, len);
				  memcpy(ini_entries + ini_entries_len + len, "=1\n\0", sizeof("=1\n\0"));
				  ini_entries_len += len + sizeof("=1\n\0") - 2;
				}
			} break;

			case 'z':
				zend_extensions_len++;
				if (zend_extensions) {
					zend_extensions = realloc(zend_extensions, sizeof(char*) * zend_extensions_len);
				} else zend_extensions = malloc(sizeof(char*) * zend_extensions_len);
				zend_extensions[zend_extensions_len-1] = strdup(jonj_optarg);
			break;

			/* begin jonjdbg options */

			case 'S': { /* set SAPI name */
				if (sapi_name) {
					free(sapi_name);
				}
				sapi_name = strdup(jonj_optarg);
			} break;

			case 'I': { /* ignore .jonjdbginit */
				init_file_default = 0;
			} break;

			case 'i': { /* set init file */
				if (init_file) {
					free(init_file);
				}

				init_file_len = strlen(jonj_optarg);
				if (init_file_len) {
					init_file = strdup(jonj_optarg);
				}
			} break;

			case 'O': { /* set oplog output */
				oplog_file_len = strlen(jonj_optarg);
				if (oplog_file_len) {
					oplog_file = strdup(jonj_optarg);
				}
			} break;

			case 'v': /* set quietness off */
				flags &= ~JONJDBG_IS_QUIET;
			break;

			case 's': /* set stepping on */
				step = 1;
			break;

			case 'E': /* stepping through eval on */
				flags |= JONJDBG_IS_STEPONEVAL;
			break;

			case 'b': /* set colours off */
				flags &= ~JONJDBG_IS_COLOURED;
			break;

			case 'q': /* hide banner */
				show_banner = 0;
			break;

#ifndef _WIN32
			/* if you pass a listen port, we will accept input on listen port */
			/* and write output to listen port * 2 */

			case 'l': { /* set listen ports */
				if (sscanf(jonj_optarg, "%d/%d", &listen[0], &listen[1]) != 2) {
					if (sscanf(jonj_optarg, "%d", &listen[0]) != 1) {
						/* default to hardcoded ports */
						listen[0] = 4000;
						listen[1] = 8000;
					} else {
						listen[1] = (listen[0] * 2);
					}
				}
			} break;

			case 'a': { /* set bind address */
				free(address);
				if (!jonj_optarg) {
					address = strdup("*");
				} else address = strdup(jonj_optarg);
			} break;
#endif

			case 'V': {
				sapi_startup(jonjdbg);
				jonjdbg->startup(jonjdbg);
				printf(
					"jonjdbg %s (built: %s %s)\nJONJ %s, Copyright (c) 1997-2016 The JONJ Group\n%s",
					JONJDBG_VERSION,
					__DATE__,
					__TIME__,
					JONJ_VERSION,
					get_zend_version()
				);
				sapi_deactivate(TSRMLS_C);
				sapi_shutdown();
				return 0;
			} break;
		}
	}
	
	/* set exec if present on command line */
	if ((argc > jonj_optind) && (strcmp(argv[jonj_optind-1],"--") != SUCCESS))
	{
		exec_len = strlen(argv[jonj_optind]);
		if (exec_len) {
			if (exec) {
				free(exec);
			}
			exec = strdup(argv[jonj_optind]);
		}
		jonj_optind++;
	}

#ifndef _WIN32
	/* setup remote server if necessary */
	if (!cleaning &&
		(listen[0] > 0 && listen[1] > 0)) {
		if (jonjdbg_open_sockets(address, listen, &server, &socket, streams) == FAILURE) {
			remote = 0;
			exit(0);
		}
		/* set remote flag to stop service shutting down upon quit */
		remote = 1;
	}
#endif

	if (sapi_name) {
		jonjdbg->name = sapi_name;
	}

	jonjdbg->ini_defaults = jonjdbg_ini_defaults;
	jonjdbg->jonjinfo_as_text = 1;
	jonjdbg->jonj_ini_ignore_cwd = 1;

	sapi_startup(jonjdbg);

	jonjdbg->executable_location = argv[0];
	jonjdbg->jonjinfo_as_text = 1;
	jonjdbg->jonj_ini_ignore = ini_ignore;
	jonjdbg->jonj_ini_path_override = ini_override;

	if (ini_entries) {
		ini_entries = realloc(ini_entries, ini_entries_len + sizeof(jonjdbg_ini_hardcoded));
		memmove(ini_entries + sizeof(jonjdbg_ini_hardcoded) - 2, ini_entries, ini_entries_len + 1);
		memcpy(ini_entries, jonjdbg_ini_hardcoded, sizeof(jonjdbg_ini_hardcoded) - 2);
	} else {
		ini_entries = malloc(sizeof(jonjdbg_ini_hardcoded));
		memcpy(ini_entries, jonjdbg_ini_hardcoded, sizeof(jonjdbg_ini_hardcoded));
	}
	ini_entries_len += sizeof(jonjdbg_ini_hardcoded) - 2;

	if (zend_extensions_len) {
		zend_ulong zend_extension = 0L;

		while (zend_extension < zend_extensions_len) {
			const char *ze = zend_extensions[zend_extension];
			size_t ze_len = strlen(ze);

			ini_entries = realloc(
				ini_entries, ini_entries_len + (ze_len + (sizeof("zend_extension=\n"))));
			memcpy(&ini_entries[ini_entries_len], "zend_extension=", (sizeof("zend_extension=\n")-1));
			ini_entries_len += (sizeof("zend_extension=")-1);
			memcpy(&ini_entries[ini_entries_len], ze, ze_len);
			ini_entries_len += ze_len;
			memcpy(&ini_entries[ini_entries_len], "\n", (sizeof("\n") - 1));

			free(zend_extensions[zend_extension]);
			zend_extension++;
		}

		free(zend_extensions);
	}

	jonjdbg->ini_entries = ini_entries;

	if (jonjdbg->startup(jonjdbg) == SUCCESS) {
#ifdef _WIN32
    EXCEPTION_POINTERS *xp;
    __try {
#endif
		zend_mm_heap *mm_heap = jonjdbg_mm_get_heap();

		if (mm_heap->use_zend_alloc) {
			mm_heap->_malloc = jonjdbg_malloc_wrapper;
			mm_heap->_realloc = jonjdbg_realloc_wrapper;
			mm_heap->_free = jonjdbg_free_wrapper;
			mm_heap->use_zend_alloc = 0;
		}

		zend_activate(TSRMLS_C);

		JONJDBG_G(original_free_function) = mm_heap->_free;
		mm_heap->_free = jonjdbg_watch_efree;

		jonjdbg_setup_watchpoints(TSRMLS_C);

#if defined(ZEND_SIGNALS) && !defined(_WIN32)
		zend_try {
			zend_signal_activate(TSRMLS_C);
		} zend_end_try();
#endif

#if defined(ZEND_SIGNALS) && !defined(_WIN32)
		zend_try { zend_sigaction(SIGSEGV, &signal_struct, &JONJDBG_G(old_sigsegv_signal) TSRMLS_CC); } zend_end_try();
		zend_try { zend_sigaction(SIGBUS, &signal_struct, &JONJDBG_G(old_sigsegv_signal) TSRMLS_CC); } zend_end_try();
#elif !defined(_WIN32)
		sigaction(SIGSEGV, &signal_struct, &JONJDBG_G(old_sigsegv_signal));
		sigaction(SIGBUS, &signal_struct, &JONJDBG_G(old_sigsegv_signal));
#endif

		if (jonj_request_startup(TSRMLS_C) == SUCCESS) {
			int i;
		
			SG(request_info).argc = argc - jonj_optind + 1;		
			SG(request_info).argv = emalloc(SG(request_info).argc * sizeof(char *));
			for (i = SG(request_info).argc; --i;) {
				SG(request_info).argv[i] = estrdup(argv[jonj_optind - 1 + i]);
			}
			SG(request_info).argv[i] = exec ? estrndup(exec, exec_len) : estrdup("");

			jonj_hash_environment(TSRMLS_C);
		}

		/* make sure to turn off buffer for ev command */
		jonj_output_activate(TSRMLS_C);
		jonj_output_deactivate(TSRMLS_C);
		
		/* do not install sigint handlers for remote consoles */
		/* sending SIGINT then provides a decent way of shutting down the server */
#ifndef _WIN32
		if (listen[0] < 0) {
#endif
#if defined(ZEND_SIGNALS) && !defined(_WIN32)
			zend_try { zend_signal(SIGINT, jonjdbg_sigint_handler TSRMLS_CC); } zend_end_try();
#else
			signal(SIGINT, jonjdbg_sigint_handler);
#endif
#ifndef _WIN32
		}
#endif

		PG(modules_activated) = 0;

		/* set flags from command line */
		JONJDBG_G(flags) = flags;

#ifndef _WIN32
		/* setup io here */
		if (streams[0] && streams[1]) {
			JONJDBG_G(flags) |= JONJDBG_IS_REMOTE;

			signal(SIGPIPE, SIG_IGN);
		}
#endif

		JONJDBG_G(io)[JONJDBG_STDIN] = stdin;
		JONJDBG_G(io)[JONJDBG_STDOUT] = stdout;
		JONJDBG_G(io)[JONJDBG_STDERR] = stderr;

		if (exec) { /* set execution context */
			JONJDBG_G(exec) = jonjdbg_resolve_path(exec TSRMLS_CC);
			JONJDBG_G(exec_len) = strlen(JONJDBG_G(exec));

			free(exec);
		}

		if (oplog_file) { /* open oplog */
			JONJDBG_G(oplog) = fopen(oplog_file, "w+");
			if (!JONJDBG_G(oplog)) {
				jonjdbg_error(
						"Failed to open oplog %s", oplog_file);
			}
			free(oplog_file);
		}

		/* set default colors */
		jonjdbg_set_color_ex(JONJDBG_COLOR_PROMPT,  JONJDBG_STRL("white-bold") TSRMLS_CC);
		jonjdbg_set_color_ex(JONJDBG_COLOR_ERROR,   JONJDBG_STRL("red-bold") TSRMLS_CC);
		jonjdbg_set_color_ex(JONJDBG_COLOR_NOTICE,  JONJDBG_STRL("green") TSRMLS_CC);

		/* set default prompt */
		jonjdbg_set_prompt(PROMPT TSRMLS_CC);

		/* Make stdin, stdout and stderr accessible from JONJ scripts */
		jonjdbg_register_file_handles(TSRMLS_C);

		if (show_banner) {
			/* print blurb */
			jonjdbg_welcome((cleaning > 0) TSRMLS_CC);
		}

		/* auto compile */
		if (JONJDBG_G(exec)) {
			jonjdbg_compile(TSRMLS_C);
		}

		/* initialize from file */
		JONJDBG_G(flags) |= JONJDBG_IS_INITIALIZING;
		zend_try {
			jonjdbg_init(init_file, init_file_len, init_file_default TSRMLS_CC);
			jonjdbg_try_file_init(bp_tmp_file, strlen(bp_tmp_file), 0 TSRMLS_CC);
		} zend_end_try();
		JONJDBG_G(flags) &= ~JONJDBG_IS_INITIALIZING;
		
		/* quit if init says so */
		if (JONJDBG_G(flags) & JONJDBG_IS_QUITTING) {
			goto jonjdbg_out;
		}

		/* step from here, not through init */
		if (step) {
			JONJDBG_G(flags) |= JONJDBG_IS_STEPPING;
		}

		if (run) {
			/* no need to try{}, run does it ... */
			JONJDBG_COMMAND_HANDLER(run)(NULL TSRMLS_CC);
			if (run > 1) {
				/* if -r is on the command line more than once just quit */
				goto jonjdbg_out;
			}
		}

/* #ifndef for making compiler shutting up */
#ifndef _WIN32
jonjdbg_interact:
#endif
		/* jonjdbg main() */
		do {
			zend_try {
				jonjdbg_interactive(TSRMLS_C);
			} zend_catch {
				if ((JONJDBG_G(flags) & JONJDBG_IS_CLEANING)) {
					FILE *bp_tmp_fp = fopen(bp_tmp_file, "w");
					jonjdbg_export_breakpoints(bp_tmp_fp TSRMLS_CC);
					fclose(bp_tmp_fp);
					cleaning = 1;
				} else {
					cleaning = 0;
				}

#ifndef _WIN32
				if (!cleaning) {
					/* remote client disconnected */
					if ((JONJDBG_G(flags) & JONJDBG_IS_DISCONNECTED)) {
					
						if (JONJDBG_G(flags) & JONJDBG_IS_REMOTE) {
							/* renegociate connections */
							jonjdbg_open_sockets(
								address, listen, &server, &socket, streams);
				
							/* set streams */
							if (streams[0] && streams[1]) {
								JONJDBG_G(flags) &= ~JONJDBG_IS_QUITTING;
							}
				
							/* this must be forced */
							CG(unclean_shutdown) = 0;
						} else {
							/* local consoles cannot disconnect, ignore EOF */
							JONJDBG_G(flags) &= ~JONJDBG_IS_DISCONNECTED;
						}
					}
				}
#endif
			} zend_end_try();
		} while(!cleaning && !(JONJDBG_G(flags) & JONJDBG_IS_QUITTING));
		
		/* this must be forced */
		CG(unclean_shutdown) = 0;
		
		/* this is just helpful */
		PG(report_memleaks) = 0;
		
#ifndef _WIN32
jonjdbg_out:
		if ((JONJDBG_G(flags) & JONJDBG_IS_DISCONNECTED)) {
			JONJDBG_G(flags) &= ~JONJDBG_IS_DISCONNECTED;
			goto jonjdbg_interact;
		}
#endif

#ifdef _WIN32
	} __except(jonjdbg_exception_handler_win32(xp = GetExceptionInformation())) {
		jonjdbg_error("Access violation (Segmentation fault) encountered\ntrying to abort cleanly...");
	}
jonjdbg_out:
#endif
	
		{
			int i;
			/* free argv */
			for (i = SG(request_info).argc; --i;) {
				efree(SG(request_info).argv[i]);
			}
			efree(SG(request_info).argv);
		}

#ifndef ZTS
		/* force cleanup of auto and core globals */
		zend_hash_clean(CG(auto_globals));
		memset(
			&core_globals, 0, sizeof(jonj_core_globals));
#endif
		if (ini_entries) {
			free(ini_entries);
		}

		if (ini_override) {
			free(ini_override);
		}
		
		/* this must be forced */
		CG(unclean_shutdown) = 0;
		
		/* this is just helpful */
		PG(report_memleaks) = 0;

		jonj_request_shutdown((void*)0);

		zend_try {
			jonj_module_shutdown(TSRMLS_C);
		} zend_end_try();

		sapi_shutdown();

	}

	if (cleaning || remote) {
		goto jonjdbg_main;
	}
	
#ifdef ZTS
	/* bugggy */
	/* tsrm_shutdown(); */
#endif

#ifndef _WIN32
	if (address) {
		free(address);
	}
#endif

	if (sapi_name) {
		free(sapi_name);
	}
	
#ifdef _WIN32
	free(bp_tmp_file);
#else
	unlink(bp_tmp_file);
#endif

	return 0;
} /* }}} */
