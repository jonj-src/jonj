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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_INI_H
#define JONJ_INI_H

#include "zend_ini.h"

BEGIN_EXTERN_C()
JONJAPI void config_zval_dtor(zval *zvalue);
int jonj_init_config(TSRMLS_D);
int jonj_shutdown_config(void);
void jonj_ini_register_extensions(TSRMLS_D);
JONJAPI zval *cfg_get_entry(const char *name, uint name_length);
JONJAPI int cfg_get_long(const char *varname, long *result);
JONJAPI int cfg_get_double(const char *varname, double *result);
JONJAPI int cfg_get_string(const char *varname, char **result);
JONJAPI int jonj_parse_user_ini_file(const char *dirname, char *ini_filename, HashTable *target_hash TSRMLS_DC);
JONJAPI void jonj_ini_activate_config(HashTable *source_hash, int modify_type, int stage TSRMLS_DC);
JONJAPI int jonj_ini_has_per_dir_config(void);
JONJAPI int jonj_ini_has_per_host_config(void);
JONJAPI void jonj_ini_activate_per_dir_config(char *path, uint path_len TSRMLS_DC);
JONJAPI void jonj_ini_activate_per_host_config(const char *host, uint host_len TSRMLS_DC);
JONJAPI HashTable* jonj_ini_get_configuration_hash(void);
END_EXTERN_C()

#define JONJ_INI_USER	ZEND_INI_USER
#define JONJ_INI_PERDIR	ZEND_INI_PERDIR
#define JONJ_INI_SYSTEM	ZEND_INI_SYSTEM

#define JONJ_INI_ALL 	ZEND_INI_ALL

#define jonj_ini_entry	zend_ini_entry

#define JONJ_INI_MH		ZEND_INI_MH
#define JONJ_INI_DISP	ZEND_INI_DISP

#define JONJ_INI_BEGIN		ZEND_INI_BEGIN
#define JONJ_INI_END			ZEND_INI_END

#define JONJ_INI_ENTRY3_EX	ZEND_INI_ENTRY3_EX
#define JONJ_INI_ENTRY3		ZEND_INI_ENTRY3
#define JONJ_INI_ENTRY2_EX	ZEND_INI_ENTRY2_EX
#define JONJ_INI_ENTRY2		ZEND_INI_ENTRY2
#define JONJ_INI_ENTRY1_EX	ZEND_INI_ENTRY1_EX
#define JONJ_INI_ENTRY1		ZEND_INI_ENTRY1
#define JONJ_INI_ENTRY_EX	ZEND_INI_ENTRY_EX
#define JONJ_INI_ENTRY		ZEND_INI_ENTRY

#define STD_JONJ_INI_ENTRY		STD_ZEND_INI_ENTRY
#define STD_JONJ_INI_ENTRY_EX	STD_ZEND_INI_ENTRY_EX
#define STD_JONJ_INI_BOOLEAN		STD_ZEND_INI_BOOLEAN

#define JONJ_INI_DISPLAY_ORIG	ZEND_INI_DISPLAY_ORIG
#define JONJ_INI_DISPLAY_ACTIVE	ZEND_INI_DISPLAY_ACTIVE

#define JONJ_INI_STAGE_STARTUP		ZEND_INI_STAGE_STARTUP
#define JONJ_INI_STAGE_SHUTDOWN		ZEND_INI_STAGE_SHUTDOWN
#define JONJ_INI_STAGE_ACTIVATE		ZEND_INI_STAGE_ACTIVATE
#define JONJ_INI_STAGE_DEACTIVATE	ZEND_INI_STAGE_DEACTIVATE
#define JONJ_INI_STAGE_RUNTIME		ZEND_INI_STAGE_RUNTIME
#define JONJ_INI_STAGE_HTACCESS		ZEND_INI_STAGE_HTACCESS

#define jonj_ini_boolean_displayer_cb	zend_ini_boolean_displayer_cb
#define jonj_ini_color_displayer_cb		zend_ini_color_displayer_cb

#define jonj_alter_ini_entry		zend_alter_ini_entry

#define jonj_ini_long	zend_ini_long
#define jonj_ini_double	zend_ini_double
#define jonj_ini_string	zend_ini_string

#endif /* JONJ_INI_H */
