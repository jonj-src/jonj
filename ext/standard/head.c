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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include "jonj.h"
#include "ext/standard/jonj_standard.h"
#include "ext/date/jonj_date.h"
#include "SAPI.h"
#include "jonj_main.h"
#include "head.h"
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "jonj_globals.h"


/* Implementation of the language Header() function */
/* {{{ proto void header(string header [, bool replace, [int http_response_code]])
   Sends a raw HTTP header */
JONJ_FUNCTION(header)
{
	zend_bool rep = 1;
	sapi_header_line ctr = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bl", &ctr.line,
				&ctr.line_len, &rep, &ctr.response_code) == FAILURE)
		return;

	sapi_header_op(rep ? SAPI_HEADER_REPLACE:SAPI_HEADER_ADD, &ctr TSRMLS_CC);
}
/* }}} */

/* {{{ proto void header_remove([string name])
   Removes an HTTP header previously set using header() */
JONJ_FUNCTION(header_remove)
{
	sapi_header_line ctr = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &ctr.line,
	                          &ctr.line_len) == FAILURE)
		return;

	sapi_header_op(ZEND_NUM_ARGS() == 0 ? SAPI_HEADER_DELETE_ALL : SAPI_HEADER_DELETE, &ctr TSRMLS_CC);
}
/* }}} */

JONJAPI int jonj_header(TSRMLS_D)
{
	if (sapi_send_headers(TSRMLS_C)==FAILURE || SG(request_info).headers_only) {
		return 0; /* don't allow output */
	} else {
		return 1; /* allow output */
	}
}


JONJAPI int jonj_setcookie(char *name, int name_len, char *value, int value_len, time_t expires, char *path, int path_len, char *domain, int domain_len, int secure, int url_encode, int httponly TSRMLS_DC)
{
	char *cookie, *encoded_value = NULL;
	int len=sizeof("Set-Cookie: ");
	char *dt;
	sapi_header_line ctr = {0};
	int result;

	if (name && strpbrk(name, "=,; \t\r\n\013\014") != NULL) {   /* man isspace for \013 and \014 */
		zend_error( E_WARNING, "Cookie names cannot contain any of the following '=,; \\t\\r\\n\\013\\014'" );
		return FAILURE;
	}

	if (!url_encode && value && strpbrk(value, ",; \t\r\n\013\014") != NULL) { /* man isspace for \013 and \014 */
		zend_error( E_WARNING, "Cookie values cannot contain any of the following ',; \\t\\r\\n\\013\\014'" );
		return FAILURE;
	}

	len += name_len;
	if (value && url_encode) {
		int encoded_value_len;

		encoded_value = jonj_url_encode(value, value_len, &encoded_value_len);
		len += encoded_value_len;
	} else if ( value ) {
		encoded_value = estrdup(value);
		len += value_len;
	}
	if (path) {
		len += path_len;
	}
	if (domain) {
		len += domain_len;
	}

	cookie = emalloc(len + 100);

	if (value == NULL || value_len == 0) {
		/*
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date in the past
		 */
		dt = jonj_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, 1, 0 TSRMLS_CC);
		snprintf(cookie, len + 100, "Set-Cookie: %s=deleted; expires=%s; Max-Age=0", name, dt);
		efree(dt);
	} else {
		snprintf(cookie, len + 100, "Set-Cookie: %s=%s", name, value ? encoded_value : "");
		if (expires > 0) {
			const char *p;
			char tsdelta[13];
			strlcat(cookie, "; expires=", len + 100);
			dt = jonj_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, expires, 0 TSRMLS_CC);
			/* check to make sure that the year does not exceed 4 digits in length */
			p = zend_memrchr(dt, '-', strlen(dt));
			if (!p || *(p + 5) != ' ') {
				efree(dt);
				efree(cookie);
				efree(encoded_value);
				zend_error(E_WARNING, "Expiry date cannot have a year greater than 9999");
				return FAILURE;
			}
			strlcat(cookie, dt, len + 100);
			efree(dt);

			snprintf(tsdelta, sizeof(tsdelta), "%li", (long) difftime(expires, time(NULL)));
			strlcat(cookie, "; Max-Age=", len + 100);
			strlcat(cookie, tsdelta, len + 100);
		}
	}

	if (encoded_value) {
		efree(encoded_value);
	}

	if (path && path_len > 0) {
		strlcat(cookie, "; path=", len + 100);
		strlcat(cookie, path, len + 100);
	}
	if (domain && domain_len > 0) {
		strlcat(cookie, "; domain=", len + 100);
		strlcat(cookie, domain, len + 100);
	}
	if (secure) {
		strlcat(cookie, "; secure", len + 100);
	}
	if (httponly) {
		strlcat(cookie, "; httponly", len + 100);
	}

	ctr.line = cookie;
	ctr.line_len = strlen(cookie);

	result = sapi_header_op(SAPI_HEADER_ADD, &ctr TSRMLS_CC);
	efree(cookie);
	return result;
}


/* jonj_set_cookie(name, value, expires, path, domain, secure) */
/* {{{ proto bool setcookie(string name [, string value [, int expires [, string path [, string domain [, bool secure[, bool httponly]]]]]])
   Send a cookie */
JONJ_FUNCTION(setcookie)
{
	char *name, *value = NULL, *path = NULL, *domain = NULL;
	long expires = 0;
	zend_bool secure = 0, httponly = 0;
	int name_len, value_len = 0, path_len = 0, domain_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|slssbb", &name,
							  &name_len, &value, &value_len, &expires, &path,
							  &path_len, &domain, &domain_len, &secure, &httponly) == FAILURE) {
		return;
	}

	if (jonj_setcookie(name, name_len, value, value_len, expires, path, path_len, domain, domain_len, secure, 1, httponly TSRMLS_CC) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto bool setrawcookie(string name [, string value [, int expires [, string path [, string domain [, bool secure[, bool httponly]]]]]])
   Send a cookie with no url encoding of the value */
JONJ_FUNCTION(setrawcookie)
{
	char *name, *value = NULL, *path = NULL, *domain = NULL;
	long expires = 0;
	zend_bool secure = 0, httponly = 0;
	int name_len, value_len = 0, path_len = 0, domain_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|slssbb", &name,
							  &name_len, &value, &value_len, &expires, &path,
							  &path_len, &domain, &domain_len, &secure, &httponly) == FAILURE) {
		return;
	}

	if (jonj_setcookie(name, name_len, value, value_len, expires, path, path_len, domain, domain_len, secure, 0, httponly TSRMLS_CC) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */


/* {{{ proto bool headers_sent([string &$file [, int &$line]])
   Returns true if headers have already been sent, false otherwise */
JONJ_FUNCTION(headers_sent)
{
	zval *arg1 = NULL, *arg2 = NULL;
	const char *file="";
	int line=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zz", &arg1, &arg2) == FAILURE)
		return;

	if (SG(headers_sent)) {
		line = jonj_output_get_start_lineno(TSRMLS_C);
		file = jonj_output_get_start_filename(TSRMLS_C);
	}

	switch(ZEND_NUM_ARGS()) {
	case 2:
		zval_dtor(arg2);
		ZVAL_LONG(arg2, line);
	case 1:
		zval_dtor(arg1);
		if (file) {
			ZVAL_STRING(arg1, file, 1);
		} else {
			ZVAL_STRING(arg1, "", 1);
		}
		break;
	}

	if (SG(headers_sent)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ jonj_head_apply_header_list_to_hash
   Turn an llist of sapi_header_struct headers into a numerically indexed zval hash */
static void jonj_head_apply_header_list_to_hash(void *data, void *arg TSRMLS_DC)
{
	sapi_header_struct *sapi_header = (sapi_header_struct *)data;

	if (arg && sapi_header) {
		add_next_index_string((zval *)arg, (char *)(sapi_header->header), 1);
	}
}

/* {{{ proto array headers_list(void)
   Return list of headers to be sent / already sent */
JONJ_FUNCTION(headers_list)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	zend_llist_apply_with_argument(&SG(sapi_headers).headers, jonj_head_apply_header_list_to_hash, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto long http_response_code([int response_code])
   Sets a response code, or returns the current HTTP response code */
JONJ_FUNCTION(http_response_code)
{
	long response_code = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &response_code) == FAILURE) {
		return;
	}

	if (response_code)
	{
		long old_response_code;

		old_response_code = SG(sapi_headers).http_response_code;
		SG(sapi_headers).http_response_code = response_code;

		if (old_response_code) {
			RETURN_LONG(old_response_code);
		}

		RETURN_TRUE;
	}

	if (!SG(sapi_headers).http_response_code) {
		RETURN_FALSE;
	}

	RETURN_LONG(SG(sapi_headers).http_response_code);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4 * End:
 */
