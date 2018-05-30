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
   | Author: Kristian Koehntopp <kris@koehntopp.de>					      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes & prototypes */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_streams.h"

#if HAVE_LIBRECODE

/* For recode 3.5 */
#if HAVE_BROKEN_RECODE
extern char *program_name;
char *program_name = "jonj";
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
  typedef enum {false = 0, true = 1} bool;
#endif

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <recode.h>

#include "jonj_recode.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/jonj_string.h"

/* }}} */

ZEND_BEGIN_MODULE_GLOBALS(recode)
    RECODE_OUTER  outer;
ZEND_END_MODULE_GLOBALS(recode)

#ifdef ZTS
# define ReSG(v) TSRMG(recode_globals_id, zend_recode_globals *, v)
#else
# define ReSG(v) (recode_globals.v)
#endif

ZEND_DECLARE_MODULE_GLOBALS(recode)
static JONJ_GINIT_FUNCTION(recode);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_recode_string, 0, 0, 2)
	ZEND_ARG_INFO(0, request)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_recode_file, 0, 0, 3)
	ZEND_ARG_INFO(0, request)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, output)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ module stuff */
static const zend_function_entry jonj_recode_functions[] = {
	JONJ_FE(recode_string, 	arginfo_recode_string)
	JONJ_FE(recode_file, 	arginfo_recode_file)
	JONJ_FALIAS(recode, recode_string, arginfo_recode_string)
	JONJ_FE_END
};

zend_module_entry recode_module_entry = {
	STANDARD_MODULE_HEADER,
	"recode",
 	jonj_recode_functions,
	JONJ_MINIT(recode),
	JONJ_MSHUTDOWN(recode),
	NULL,
	NULL,
	JONJ_MINFO(recode),
	NO_VERSION_YET,
	JONJ_MODULE_GLOBALS(recode),
	JONJ_GINIT(recode),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_RECODE
ZEND_GET_MODULE(recode)
#endif

static JONJ_GINIT_FUNCTION(recode)
{
	recode_globals->outer = NULL;
}

JONJ_MINIT_FUNCTION(recode)
{
	ReSG(outer) = recode_new_outer(false);
	if (ReSG(outer) == NULL) {
		return FAILURE;
	}

	return SUCCESS;
}

JONJ_MSHUTDOWN_FUNCTION(recode)
{
	if (ReSG(outer)) {
		recode_delete_outer(ReSG(outer));
	}
	return SUCCESS;
}

JONJ_MINFO_FUNCTION(recode)
{
	jonj_info_print_table_start();
	jonj_info_print_table_row(2, "Recode Support", "enabled");
	jonj_info_print_table_row(2, "Revision", "$Id$");
	jonj_info_print_table_end();
}

/* {{{ proto string recode_string(string request, string str)
   Recode string str according to request string */
JONJ_FUNCTION(recode_string)
{
	RECODE_REQUEST request = NULL;
	char *r = NULL;
	size_t r_len = 0, r_alen = 0;
	int req_len, str_len;
	char *req, *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &req, &req_len, &str, &str_len) == FAILURE) {
		return;
	}

	request = recode_new_request(ReSG(outer));

	if (request == NULL) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot allocate request structure");
		RETURN_FALSE;
	}

	if (!recode_scan_request(request, req)) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal recode request '%s'", req);
		goto error_exit;
	}

	recode_buffer_to_buffer(request, str, str_len, &r, &r_len, &r_alen);
	if (!r) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Recoding failed.");
error_exit:
		RETVAL_FALSE;
	} else {
		RETVAL_STRINGL_CHECK(r, r_len, 1);
		free(r);
	}

	recode_delete_request(request);

	return;
}
/* }}} */

/* {{{ proto bool recode_file(string request, resource input, resource output)
   Recode file input into file output according to request */
JONJ_FUNCTION(recode_file)
{
	RECODE_REQUEST request = NULL;
	char *req;
	int req_len;
	zval *input, *output;
	jonj_stream *instream, *outstream;
	FILE  *in_fp,  *out_fp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "srr", &req, &req_len, &input, &output) == FAILURE) {
	 	return;
	}

	jonj_stream_from_zval(instream, &input);
	jonj_stream_from_zval(outstream, &output);

	if (FAILURE == jonj_stream_cast(instream, JONJ_STREAM_AS_STDIO, (void**)&in_fp, REPORT_ERRORS))	{
		RETURN_FALSE;
	}

	if (FAILURE == jonj_stream_cast(outstream, JONJ_STREAM_AS_STDIO, (void**)&out_fp, REPORT_ERRORS))	{
		RETURN_FALSE;
	}

	request = recode_new_request(ReSG(outer));
	if (request == NULL) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot allocate request structure");
		RETURN_FALSE;
	}

	if (!recode_scan_request(request, req)) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal recode request '%s'", req);
		goto error_exit;
	}

	if (!recode_file_to_file(request, in_fp, out_fp)) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Recoding failed.");
		goto error_exit;
	}

	recode_delete_request(request);
	RETURN_TRUE;

error_exit:
	recode_delete_request(request);
	RETURN_FALSE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
