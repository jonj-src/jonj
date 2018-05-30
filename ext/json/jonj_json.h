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
  | Author: Omar Kilani <omar@jonj.tk>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_JSON_H
#define JONJ_JSON_H

#define JONJ_JSON_VERSION "1.2.1"
#include "ext/standard/jonj_smart_str.h"

extern zend_module_entry json_module_entry;
#define jonjext_json_ptr &json_module_entry

#if defined(JONJ_WIN32) && defined(JSON_EXPORTS)
#define JONJ_JSON_API __declspec(dllexport)
#else
#define JONJ_JSON_API JONJAPI
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(json)
	int encoder_depth;
	int error_code;
	int encode_max_depth;
ZEND_END_MODULE_GLOBALS(json)

#ifdef ZTS
# define JSON_G(v) TSRMG(json_globals_id, zend_json_globals *, v)
#else
# define JSON_G(v) (json_globals.v)
#endif

JONJ_JSON_API void jonj_json_encode(smart_str *buf, zval *val, int options TSRMLS_DC);
JONJ_JSON_API void jonj_json_decode_ex(zval *return_value, char *str, int str_len, int options, long depth TSRMLS_DC);
extern JONJ_JSON_API zend_class_entry *jonj_json_serializable_ce;


/* json_encode() options */
#define JONJ_JSON_HEX_TAG	(1<<0)
#define JONJ_JSON_HEX_AMP	(1<<1)
#define JONJ_JSON_HEX_APOS	(1<<2)
#define JONJ_JSON_HEX_QUOT	(1<<3)
#define JONJ_JSON_FORCE_OBJECT	(1<<4)
#define JONJ_JSON_NUMERIC_CHECK	(1<<5)
#define JONJ_JSON_UNESCAPED_SLASHES	(1<<6)
#define JONJ_JSON_PRETTY_PRINT	(1<<7)
#define JONJ_JSON_UNESCAPED_UNICODE	(1<<8)
#define JONJ_JSON_PARTIAL_OUTPUT_ON_ERROR (1<<9)
#define JONJ_JSON_PRESERVE_ZERO_FRACTION (1<<10)

/* Internal flags */
#define JONJ_JSON_OUTPUT_ARRAY	0
#define JONJ_JSON_OUTPUT_OBJECT	1

/* json_decode() options */
#define JONJ_JSON_OBJECT_AS_ARRAY	(1<<0)
#define JONJ_JSON_BIGINT_AS_STRING	(1<<1)

static inline void jonj_json_decode(zval *return_value, char *str, int str_len, zend_bool assoc, long depth TSRMLS_DC)
{
	jonj_json_decode_ex(return_value, str, str_len, assoc ? JONJ_JSON_OBJECT_AS_ARRAY : 0, depth TSRMLS_CC);
}


#endif  /* JONJ_JSON_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
