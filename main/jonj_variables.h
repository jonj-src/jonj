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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_VARIABLES_H
#define JONJ_VARIABLES_H

#include "jonj.h"
#include "SAPI.h"

#define PARSE_POST 0
#define PARSE_GET 1
#define PARSE_COOKIE 2
#define PARSE_STRING 3
#define PARSE_ENV 4
#define PARSE_SERVER 5
#define PARSE_SESSION 6

BEGIN_EXTERN_C()
void jonj_startup_auto_globals(TSRMLS_D);
extern JONJAPI void (*jonj_import_environment_variables)(zval *array_ptr TSRMLS_DC);
JONJAPI void jonj_register_variable(char *var, char *val, zval *track_vars_array TSRMLS_DC);
/* binary-safe version */
JONJAPI void jonj_register_variable_safe(char *var, char *val, int val_len, zval *track_vars_array TSRMLS_DC);
JONJAPI void jonj_register_variable_ex(char *var, zval *val, zval *track_vars_array TSRMLS_DC);

JONJAPI int jonj_hash_environment(TSRMLS_D);
END_EXTERN_C()

#define NUM_TRACK_VARS	6

#endif /* JONJ_VARIABLES_H */
