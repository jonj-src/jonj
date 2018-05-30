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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_COM_DOTNET_H
#define JONJ_COM_DOTNET_H

extern zend_module_entry com_dotnet_module_entry;
#define jonjext_com_dotnet_ptr &com_dotnet_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef JONJ_WIN32
# define JONJ_COM_DOTNET_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define JONJ_COM_DOTNET_API __attribute__ ((visibility("default")))
#else
# define JONJ_COM_DOTNET_API
#endif

JONJ_MINIT_FUNCTION(com_dotnet);
JONJ_MSHUTDOWN_FUNCTION(com_dotnet);
JONJ_RINIT_FUNCTION(com_dotnet);
JONJ_RSHUTDOWN_FUNCTION(com_dotnet);
JONJ_MINFO_FUNCTION(com_dotnet);

ZEND_BEGIN_MODULE_GLOBALS(com_dotnet)
	zend_bool allow_dcom;
	zend_bool autoreg_verbose;
	zend_bool autoreg_on;
	zend_bool autoreg_case_sensitive;
	void *dotnet_runtime_stuff; /* opaque to avoid cluttering up other modules */
	int code_page; /* default code_page if left unspecified */
	zend_bool rshutdown_started;
ZEND_END_MODULE_GLOBALS(com_dotnet)

#ifdef ZTS
# define COMG(v) TSRMG(com_dotnet_globals_id, zend_com_dotnet_globals *, v)
#else
# define COMG(v) (com_dotnet_globals.v)
#endif

extern ZEND_DECLARE_MODULE_GLOBALS(com_dotnet);

#endif	/* JONJ_COM_DOTNET_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
