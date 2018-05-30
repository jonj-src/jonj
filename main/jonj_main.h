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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_MAIN_H
#define JONJ_MAIN_H

#include "zend_globals.h"
#include "jonj_globals.h"
#include "SAPI.h"

BEGIN_EXTERN_C()
JONJAPI int jonj_request_startup(TSRMLS_D);
JONJAPI void jonj_request_shutdown(void *dummy);
JONJAPI void jonj_request_shutdown_for_exec(void *dummy);
JONJAPI int jonj_module_startup(sapi_module_struct *sf, zend_module_entry *additional_modules, uint num_additional_modules);
JONJAPI void jonj_module_shutdown(TSRMLS_D);
JONJAPI void jonj_module_shutdown_for_exec(void);
JONJAPI int jonj_module_shutdown_wrapper(sapi_module_struct *sapi_globals);
JONJAPI int jonj_request_startup_for_hook(TSRMLS_D);
JONJAPI void jonj_request_shutdown_for_hook(void *dummy);

JONJAPI int jonj_register_extensions(zend_module_entry **ptr, int count TSRMLS_DC);

JONJAPI int jonj_execute_script(zend_file_handle *primary_file TSRMLS_DC);
JONJAPI int jonj_execute_simple_script(zend_file_handle *primary_file, zval **ret TSRMLS_DC);
JONJAPI int jonj_handle_special_queries(TSRMLS_D);
JONJAPI int jonj_lint_script(zend_file_handle *file TSRMLS_DC);

JONJAPI void jonj_handle_aborted_connection(void);
JONJAPI int jonj_handle_auth_data(const char *auth TSRMLS_DC);

JONJAPI void jonj_html_puts(const char *str, uint siz TSRMLS_DC);
JONJAPI int jonj_stream_open_for_zend_ex(const char *filename, zend_file_handle *handle, int mode TSRMLS_DC);

/* environment module */
extern int jonj_init_environ(void);
extern int jonj_shutdown_environ(void);
END_EXTERN_C()

#endif
