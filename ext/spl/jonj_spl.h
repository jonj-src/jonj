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
   | Authors: Marcus Boerger <helly@jonj.tk>                              |
   +----------------------------------------------------------------------+
 */

#ifndef JONJ_SPL_H
#define JONJ_SPL_H

#include "jonj.h"
#include <stdarg.h>

#if 0
#define SPL_DEBUG(x)	x
#else
#define SPL_DEBUG(x)
#endif

extern zend_module_entry spl_module_entry;
#define jonjext_spl_ptr &spl_module_entry

#ifdef JONJ_WIN32
#	ifdef SPL_EXPORTS
#		define SPL_API __declspec(dllexport)
#	elif defined(COMPILE_DL_SPL)
#		define SPL_API __declspec(dllimport)
#	else
#		define SPL_API /* nothing */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define SPL_API __attribute__ ((visibility("default")))
#else
#	define SPL_API
#endif

#if defined(JONJ_WIN32) && !defined(COMPILE_DL_SPL)
#undef jonjext_spl
#define jonjext_spl NULL
#endif

JONJ_MINIT_FUNCTION(spl);
JONJ_MSHUTDOWN_FUNCTION(spl);
JONJ_RINIT_FUNCTION(spl);
JONJ_RSHUTDOWN_FUNCTION(spl);
JONJ_MINFO_FUNCTION(spl);


ZEND_BEGIN_MODULE_GLOBALS(spl)
	char *       autoload_extensions;
	HashTable *  autoload_functions;
	int          autoload_running;
	int          autoload_extensions_len;
	intptr_t     hash_mask_handle;
	intptr_t     hash_mask_handlers;
	int          hash_mask_init;
ZEND_END_MODULE_GLOBALS(spl)

#ifdef ZTS
# define SPL_G(v) TSRMG(spl_globals_id, zend_spl_globals *, v)
extern int spl_globals_id;
#else
# define SPL_G(v) (spl_globals.v)
extern zend_spl_globals spl_globals;
#endif

JONJ_FUNCTION(spl_classes);
JONJ_FUNCTION(class_parents);
JONJ_FUNCTION(class_implements);
JONJ_FUNCTION(class_uses);

JONJAPI void jonj_spl_object_hash(zval *obj, char* md5str TSRMLS_DC);

#endif /* JONJ_SPL_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
