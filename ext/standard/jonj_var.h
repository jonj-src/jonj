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
   | Author: Jani Lehtimäki <jkl@njet.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_VAR_H
#define JONJ_VAR_H

#include "ext/standard/basic_functions.h"
#include "ext/standard/jonj_smart_str_public.h"

JONJ_FUNCTION(var_dump);
JONJ_FUNCTION(var_export);
JONJ_FUNCTION(debug_zval_dump);
JONJ_FUNCTION(serialize);
JONJ_FUNCTION(unserialize);
JONJ_FUNCTION(memory_get_usage);
JONJ_FUNCTION(memory_get_peak_usage);

JONJAPI void jonj_var_dump(zval **struc, int level TSRMLS_DC);
JONJAPI void jonj_var_export(zval **struc, int level TSRMLS_DC);
JONJAPI void jonj_var_export_ex(zval **struc, int level, smart_str *buf TSRMLS_DC);

JONJAPI void jonj_debug_zval_dump(zval **struc, int level TSRMLS_DC);

typedef HashTable* jonj_serialize_data_t;

struct jonj_unserialize_data {
	void *first;
	void *last;
	void *first_dtor;
	void *last_dtor;
};

typedef struct jonj_unserialize_data* jonj_unserialize_data_t;

JONJAPI void jonj_var_serialize(smart_str *buf, zval **struc, jonj_serialize_data_t *var_hash TSRMLS_DC);
JONJAPI int jonj_var_unserialize(zval **rval, const unsigned char **p, const unsigned char *max, jonj_unserialize_data_t *var_hash TSRMLS_DC);

#define JONJ_VAR_SERIALIZE_INIT(var_hash_ptr) \
do  { \
	/* fprintf(stderr, "SERIALIZE_INIT      == lock: %u, level: %u\n", BG(serialize_lock), BG(serialize).level); */ \
	if (BG(serialize_lock) || !BG(serialize).level) { \
		ALLOC_HASHTABLE(var_hash_ptr); \
		zend_hash_init((var_hash_ptr), 10, NULL, NULL, 0); \
		if (!BG(serialize_lock)) { \
			BG(serialize).var_hash = (void *)(var_hash_ptr); \
			BG(serialize).level = 1; \
		} \
	} else { \
		(var_hash_ptr) = (jonj_serialize_data_t)BG(serialize).var_hash; \
		++BG(serialize).level; \
	} \
} while(0)

#define JONJ_VAR_SERIALIZE_DESTROY(var_hash_ptr) \
do { \
	/* fprintf(stderr, "SERIALIZE_DESTROY   == lock: %u, level: %u\n", BG(serialize_lock), BG(serialize).level); */ \
	if (BG(serialize_lock) || !BG(serialize).level) { \
		zend_hash_destroy((var_hash_ptr)); \
		FREE_HASHTABLE(var_hash_ptr); \
	} else { \
		if (!--BG(serialize).level) { \
			zend_hash_destroy((jonj_serialize_data_t)BG(serialize).var_hash); \
			FREE_HASHTABLE((jonj_serialize_data_t)BG(serialize).var_hash); \
			BG(serialize).var_hash = NULL; \
		} \
	} \
} while (0)

#define JONJ_VAR_UNSERIALIZE_INIT(var_hash_ptr) \
do { \
	/* fprintf(stderr, "UNSERIALIZE_INIT    == lock: %u, level: %u\n", BG(serialize_lock), BG(unserialize).level); */ \
	if (BG(serialize_lock) || !BG(unserialize).level) { \
		(var_hash_ptr) = (jonj_unserialize_data_t)ecalloc(1, sizeof(struct jonj_unserialize_data)); \
		if (!BG(serialize_lock)) { \
			BG(unserialize).var_hash = (void *)(var_hash_ptr); \
			BG(unserialize).level = 1; \
		} \
	} else { \
		(var_hash_ptr) = (jonj_unserialize_data_t)BG(unserialize).var_hash; \
		++BG(unserialize).level; \
	} \
} while (0)

#define JONJ_VAR_UNSERIALIZE_DESTROY(var_hash_ptr) \
do { \
	/* fprintf(stderr, "UNSERIALIZE_DESTROY == lock: %u, level: %u\n", BG(serialize_lock), BG(unserialize).level); */ \
	if (BG(serialize_lock) || !BG(unserialize).level) { \
		var_destroy(&(var_hash_ptr)); \
		efree(var_hash_ptr); \
	} else { \
		if (!--BG(unserialize).level) { \
			var_destroy(&(var_hash_ptr)); \
			efree((var_hash_ptr)); \
			BG(unserialize).var_hash = NULL; \
		} \
	} \
} while (0)

JONJAPI void var_replace(jonj_unserialize_data_t *var_hash, zval *ozval, zval **nzval);
JONJAPI void var_push_dtor(jonj_unserialize_data_t *var_hash, zval **val);
JONJAPI void var_push_dtor_no_addref(jonj_unserialize_data_t *var_hashx, zval **rval);
JONJAPI void var_destroy(jonj_unserialize_data_t *var_hash);

#define JONJ_VAR_UNSERIALIZE_ZVAL_CHANGED(var_hash, ozval, nzval) \
	var_replace((var_hash), (ozval), &(nzval))
	
JONJAPI zend_class_entry *jonj_create_empty_class(char *class_name, int len);

static inline int jonj_varname_check(char *name, int name_len, zend_bool silent TSRMLS_DC) /* {{{ */
{
    if (name_len == sizeof("GLOBALS") - 1 && !memcmp(name, "GLOBALS", sizeof("GLOBALS") - 1)) {
		if (!silent) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted GLOBALS variable overwrite");
		}
        return FAILURE;
    } else if (name[0] == '_' &&
            (
             (name_len == sizeof("_GET") - 1 && !memcmp(name, "_GET", sizeof("_GET") - 1)) ||
             (name_len == sizeof("_POST") - 1 && !memcmp(name, "_POST", sizeof("_POST") - 1)) ||
             (name_len == sizeof("_COOKIE") - 1 && !memcmp(name, "_COOKIE", sizeof("_COOKIE") - 1)) ||
             (name_len == sizeof("_ENV") - 1 && !memcmp(name, "_ENV", sizeof("_ENV") - 1)) ||
             (name_len == sizeof("_SERVER") - 1 && !memcmp(name, "_SERVER", sizeof("_SERVER") - 1)) ||
             (name_len == sizeof("_SESSION") - 1 && !memcmp(name, "_SESSION", sizeof("_SESSION") - 1)) ||
             (name_len == sizeof("_FILES") - 1  && !memcmp(name, "_FILES", sizeof("_FILES") - 1)) ||
             (name_len == sizeof("_REQUEST") -1 && !memcmp(name, "_REQUEST", sizeof("_REQUEST") - 1))
            )
            ) {
		if (!silent) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted super-global (%s) variable overwrite", name);
		}
        return FAILURE;
    } else if (name[0] == 'H' &&
            (
             (name_len == sizeof("HTTP_POST_VARS") - 1 && !memcmp(name, "HTTP_POST_VARS", sizeof("HTTP_POST_VARS") - 1)) ||
             (name_len == sizeof("HTTP_GET_VARS") - 1 && !memcmp(name, "HTTP_GET_VARS", sizeof("HTTP_GET_VARS") - 1)) ||
             (name_len == sizeof("HTTP_COOKIE_VARS") - 1 && !memcmp(name, "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS") - 1)) ||
             (name_len == sizeof("HTTP_ENV_VARS") - 1 && !memcmp(name, "HTTP_ENV_VARS", sizeof("HTTP_ENV_VARS") - 1)) ||
             (name_len == sizeof("HTTP_SERVER_VARS") - 1 && !memcmp(name, "HTTP_SERVER_VARS", sizeof("HTTP_SERVER_VARS") - 1)) ||
             (name_len == sizeof("HTTP_SESSION_VARS") - 1 && !memcmp(name, "HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS") - 1)) ||
             (name_len == sizeof("HTTP_RAW_POST_DATA") - 1 && !memcmp(name, "HTTP_RAW_POST_DATA", sizeof("HTTP_RAW_POST_DATA") - 1)) ||
             (name_len == sizeof("HTTP_POST_FILES") - 1 && !memcmp(name, "HTTP_POST_FILES", sizeof("HTTP_POST_FILES") - 1))
            )
            ) {
		if (!silent) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted long input array (%s) overwrite", name);
		}
        return FAILURE;
    }
	return SUCCESS;
}
/* }}} */

#endif /* JONJ_VAR_H */
