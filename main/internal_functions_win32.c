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

/* {{{ includes
 */
#include "jonj.h"
#include "jonj_main.h"
#include "zend_modules.h"
#include "zend_compile.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef ZEND_ENGINE_2
#error HEAD does not work with ZendEngine1 anymore
#endif

#include "ext/standard/dl.h"
#include "ext/standard/file.h"
#include "ext/standard/fsock.h"
#include "ext/standard/head.h"
#include "ext/standard/pack.h"
#include "ext/standard/jonj_browscap.h"
#include "ext/standard/jonj_crypt.h"
#include "ext/standard/jonj_dir.h"
#include "ext/standard/jonj_filestat.h"
#include "ext/standard/jonj_mail.h"
#include "ext/standard/jonj_ext_syslog.h"
#include "ext/standard/jonj_standard.h"
#include "ext/standard/jonj_lcg.h"
#include "ext/standard/jonj_array.h"
#include "ext/standard/jonj_assert.h"
#include "ext/reflection/jonj_reflection.h"
#if HAVE_BCMATH
#include "ext/bcmath/jonj_bcmath.h"
#endif
#if HAVE_CALENDAR
#include "ext/calendar/jonj_calendar.h"
#endif
#if HAVE_CTYPE
#include "ext/ctype/jonj_ctype.h"
#endif
#if HAVE_DATE
#include "ext/date/jonj_date.h"
#endif
#if HAVE_FTP
#include "ext/ftp/jonj_ftp.h"
#endif
#if HAVE_ICONV
#include "ext/iconv/jonj_iconv.h"
#endif
#include "ext/standard/reg.h"
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
#include "ext/pcre/jonj_pcre.h"
#endif
#if HAVE_UODBC
#include "ext/odbc/jonj_odbc.h"
#endif
#if HAVE_JONJ_SESSION
#include "ext/session/jonj_session.h"
#endif
#if HAVE_MBSTRING
#include "ext/mbstring/mbstring.h"
#endif
#if HAVE_TOKENIZER
#include "ext/tokenizer/jonj_tokenizer.h"
#endif
#if HAVE_ZLIB
#include "ext/zlib/jonj_zlib.h"
#endif
#if HAVE_LIBXML
#include "ext/libxml/jonj_libxml.h"
#if HAVE_DOM
#include "ext/dom/jonj_dom.h"
#endif
#if HAVE_SIMPLEXML
#include "ext/simplexml/jonj_simplexml.h"
#endif
#endif
#if HAVE_XML
#include "ext/xml/jonj_xml.h"
#endif
#if HAVE_XML && HAVE_WDDX
#include "ext/wddx/jonj_wddx.h"
#endif
#include "ext/com_dotnet/jonj_com_dotnet.h"
#ifdef HAVE_SPL
#include "ext/spl/jonj_spl.h"
#endif
#if HAVE_XML && HAVE_XMLREADER
#include "ext/xmlreader/jonj_xmlreader.h"
#endif
#if HAVE_XML && HAVE_XMLWRITER
#include "ext/xmlwriter/jonj_xmlwriter.h"
#endif
/* }}} */

/* {{{ jonj_builtin_extensions[]
 */
static zend_module_entry *jonj_builtin_extensions[] = {
	jonjext_standard_ptr
#if HAVE_BCMATH
	,jonjext_bcmath_ptr
#endif
#if HAVE_CALENDAR
	,jonjext_calendar_ptr
#endif
	,jonjext_com_dotnet_ptr
#if HAVE_CTYPE
	,jonjext_ctype_ptr
#endif
#if HAVE_DATE
	,jonjext_date_ptr
#endif
#if HAVE_FTP
	,jonjext_ftp_ptr
#endif
#if HAVE_HASH
	,jonjext_hash_ptr
#endif
#if HAVE_ICONV
	,jonjext_iconv_ptr
#endif
#if HAVE_MBSTRING
	,jonjext_mbstring_ptr
#endif
#if HAVE_UODBC
	,jonjext_odbc_ptr
#endif
#if HAVE_PCRE || HAVE_BUNDLED_PCRE
	,jonjext_pcre_ptr
#endif
	,jonjext_reflection_ptr
#if HAVE_JONJ_SESSION
	,jonjext_session_ptr
#endif
#if HAVE_TOKENIZER
	,jonjext_tokenizer_ptr
#endif
#if HAVE_ZLIB
	,jonjext_zlib_ptr
#endif
#if HAVE_LIBXML
	,jonjext_libxml_ptr
#if HAVE_DOM
	,jonjext_dom_ptr
#endif
#if HAVE_SIMPLEXML
	,jonjext_simplexml_ptr
#endif
#endif
#if HAVE_XML
	,jonjext_xml_ptr
#endif
#if HAVE_XML && HAVE_WDDX
	,jonjext_wddx_ptr
#endif
#if HAVE_SPL
	,jonjext_spl_ptr
#endif
#if HAVE_XML && HAVE_XMLREADER
	,jonjext_xmlreader_ptr
#endif
#if HAVE_XML && HAVE_XMLWRITER
	,jonjext_xmlwriter_ptr
#endif
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
