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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/


/* $Id$ */

#ifndef EREG_H
#define EREG_H

#include "jonj_regex.h"

extern zend_module_entry ereg_module_entry;
#define jonjext_ereg_ptr &ereg_module_entry

#ifdef JONJ_WIN32
# define JONJ_EREG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define JONJ_EREG_API __attribute__ ((visibility("default")))
#else
# define JONJ_EREG_API
#endif

JONJ_EREG_API char *jonj_ereg_replace(const char *pattern, const char *replace, const char *string, int icase, int extended TSRMLS_DC);

JONJ_FUNCTION(ereg);
JONJ_FUNCTION(eregi);
JONJ_FUNCTION(eregi_replace);
JONJ_FUNCTION(ereg_replace);
JONJ_FUNCTION(split);
JONJ_FUNCTION(spliti);
JONJ_EREG_API JONJ_FUNCTION(sql_regcase);

ZEND_BEGIN_MODULE_GLOBALS(ereg)
	HashTable ht_rc;
	unsigned int lru_counter;
ZEND_END_MODULE_GLOBALS(ereg)

/* Module functions */
JONJ_MINFO_FUNCTION(ereg);

#ifdef ZTS
#define EREG(v) TSRMG(ereg_globals_id, zend_ereg_globals *, v)
#else
#define EREG(v) (ereg_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(ereg)

#endif /* REG_H */
