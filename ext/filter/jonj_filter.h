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
  | Authors: Rasmus Lerdorf <rasmus@jonj.tk>                             |
  |          Derick Rethans <derick@jonj.tk>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_FILTER_H
#define JONJ_FILTER_H

#include "SAPI.h"
#include "zend_API.h"
#include "jonj.h"
#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/jonj_string.h"
#include "ext/standard/html.h"
#include "jonj_variables.h"

extern zend_module_entry filter_module_entry;
#define jonjext_filter_ptr &filter_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_MINIT_FUNCTION(filter);
JONJ_MSHUTDOWN_FUNCTION(filter);
JONJ_RINIT_FUNCTION(filter);
JONJ_RSHUTDOWN_FUNCTION(filter);
JONJ_MINFO_FUNCTION(filter);

JONJ_FUNCTION(filter_input);
JONJ_FUNCTION(filter_var);
JONJ_FUNCTION(filter_input_array);
JONJ_FUNCTION(filter_var_array);
JONJ_FUNCTION(filter_list);
JONJ_FUNCTION(filter_has_var);
JONJ_FUNCTION(filter_id);

ZEND_BEGIN_MODULE_GLOBALS(filter)
	zval *post_array;
	zval *get_array;
	zval *cookie_array;
	zval *env_array;
	zval *server_array;
	zval *session_array;
	long  default_filter;
	long  default_filter_flags;
ZEND_END_MODULE_GLOBALS(filter)

#ifdef ZTS
#define IF_G(v) TSRMG(filter_globals_id, zend_filter_globals *, v)
#else
#define IF_G(v) (filter_globals.v)
#endif


#define JONJ_INPUT_FILTER_PARAM_DECL zval *value, long flags, zval *option_array, char *charset TSRMLS_DC
void jonj_filter_int(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_boolean(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_float(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_validate_regexp(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_validate_url(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_validate_email(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_validate_ip(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_validate_mac(JONJ_INPUT_FILTER_PARAM_DECL);

void jonj_filter_string(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_encoded(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_special_chars(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_full_special_chars(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_unsafe_raw(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_email(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_url(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_number_int(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_number_float(JONJ_INPUT_FILTER_PARAM_DECL);
void jonj_filter_magic_quotes(JONJ_INPUT_FILTER_PARAM_DECL);

void jonj_filter_callback(JONJ_INPUT_FILTER_PARAM_DECL);

#endif	/* FILTER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
