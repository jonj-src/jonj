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
   | Author: Pierrick Charron <pierrick@jonj.tk>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"

#if HAVE_CURL

#include "jonj_curl.h"

#include <curl/curl.h>

/* {{{ proto void curl_share_init() 
   Initialize a share curl handle */
JONJ_FUNCTION(curl_share_init)
{
	jonj_curlsh *sh;
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	sh = ecalloc(1, sizeof(jonj_curlsh));

	sh->share = curl_share_init();

	ZEND_REGISTER_RESOURCE(return_value, sh, le_curl_share_handle);
}
/* }}} */

/* {{{ proto void curl_share_close(resource sh)
   Close a set of cURL handles */
JONJ_FUNCTION(curl_share_close)
{
	zval *z_sh;
	jonj_curlsh *sh;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_sh) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(sh, jonj_curlsh *, &z_sh, -1, le_curl_share_handle_name, le_curl_share_handle);
	zend_list_delete(Z_LVAL_P(z_sh));
}
/* }}} */

static int _jonj_curl_share_setopt(jonj_curlsh *sh, long option, zval **zvalue, zval *return_value TSRMLS_DC) /* {{{ */
{
	CURLSHcode error = CURLSHE_OK;

	switch (option) {
		case CURLSHOPT_SHARE:
		case CURLSHOPT_UNSHARE:
			convert_to_long_ex(zvalue);
			error = curl_share_setopt(sh->share, option, Z_LVAL_PP(zvalue));
			break;

		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid curl share configuration option");
			error = CURLSHE_BAD_OPTION; 
			break;
	}

	if (error != CURLSHE_OK) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

/* {{{ proto bool curl_share_setopt(resource sh, int option, mixed value)
      Set an option for a cURL transfer */
JONJ_FUNCTION(curl_share_setopt)
{
	zval       *zid, **zvalue;
	long        options;
	jonj_curlsh *sh;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlZ", &zid, &options, &zvalue) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(sh, jonj_curlsh *, &zid, -1, le_curl_share_handle_name, le_curl_share_handle);

	if (!_jonj_curl_share_setopt(sh, options, zvalue, return_value TSRMLS_CC)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

void _jonj_curl_share_close(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	jonj_curlsh *sh = (jonj_curlsh *) rsrc->ptr;
	if (sh) {
		curl_share_cleanup(sh->share);
		efree(sh);
		rsrc->ptr = NULL;
	}
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
