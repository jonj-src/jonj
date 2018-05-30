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
   | Authors: Sara Golemon <pollita@jonj.tk>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HTTP_H
#define JONJ_HTTP_H

#include "jonj.h"
#include "jonj_smart_str.h"

JONJAPI int jonj_url_encode_hash_ex(HashTable *ht, smart_str *formstr,
				const char *num_prefix, int num_prefix_len,
				const char *key_prefix, int key_prefix_len,
				const char *key_suffix, int key_suffix_len, 
				zval *type, char *arg_sep, int enc_type TSRMLS_DC);
#define jonj_url_encode_hash(ht, formstr)	jonj_url_encode_hash_ex((ht), (formstr), NULL, 0, NULL, 0, NULL, 0, NULL TSRMLS_CC)

JONJ_FUNCTION(http_build_query);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

