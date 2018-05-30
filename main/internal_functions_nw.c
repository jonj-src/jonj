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
   | Modified for NetWare: Novell, Inc.                                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes
 */
#include "jonj.h"
#include "jonj_main.h"
#include "zend_modules.h"
#include "zend_compile.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "ext/bcmath/jonj_bcmath.h"
#include "ext/gd/jonj_gd.h"
#include "ext/standard/dl.h"
#include "ext/standard/file.h"
#include "ext/standard/fsock.h"
#include "ext/standard/head.h"
#include "ext/standard/pack.h"
#include "ext/standard/jonj_browscap.h"
/*#include "ext/standard/jonj_crypt.h"*/
#include "ext/standard/jonj_dir.h"
#include "ext/standard/jonj_filestat.h"
#include "ext/standard/jonj_mail.h"
/*#include "ext/standard/jonj_ext_syslog.h"*/
#include "ext/standard/jonj_standard.h"
#include "ext/standard/jonj_lcg.h"
#include "ext/standard/jonj_array.h"
#include "ext/standard/jonj_assert.h"
#include "ext/calendar/jonj_calendar.h"
/*#include "ext/com/jonj_COM.h"
#include "ext/com/jonj_VARIANT.h"*/
#include "ext/ftp/jonj_ftp.h"
#include "ext/standard/reg.h"
#include "ext/pcre/jonj_pcre.h"
/*#include "ext/odbc/jonj_odbc.h"*/	/* Commented out for now */
#include "ext/session/jonj_session.h"
/*#include "ext/xml/jonj_xml.h"
#include "ext/wddx/jonj_wddx.h"
#include "ext/mysql/jonj_mysql.h"*/	/* Commented out for now */
/* }}} */

/* {{{ jonj_builtin_extensions[]
 */
static zend_module_entry *jonj_builtin_extensions[] = {
	jonjext_standard_ptr,
#if HAVE_BCMATH
	jonjext_bcmath_ptr,
#endif
	jonjext_calendar_ptr,
/*	COM_module_ptr,*/
	jonjext_ftp_ptr,
#if defined(MBSTR_ENC_TRANS)
	jonjext_mbstring_ptr,
#endif
/*	jonjext_mysql_ptr,*/	/* Commented out for now */
/*	jonjext_odbc_ptr, */	/* Commented out for now */
	jonjext_pcre_ptr,
	jonjext_session_ptr,
/*	jonjext_xml_ptr,
	jonjext_wddx_ptr */	/* Commented out for now */
};
/* }}} */

#define EXTCOUNT (sizeof(jonj_builtin_extensions)/sizeof(zend_module_entry *))

JONJAPI int jonj_register_internal_extensions(TSRMLS_D)
{
	return jonj_register_extensions(jonj_builtin_extensions, EXTCOUNT TSRMLS_CC);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
