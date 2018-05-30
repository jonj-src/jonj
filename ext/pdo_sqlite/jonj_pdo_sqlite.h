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
  | Author: Wez Furlong <wez@jonj.tk>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */


#ifndef JONJ_PDO_SQLITE_H
#define JONJ_PDO_SQLITE_H

extern zend_module_entry pdo_sqlite_module_entry;
#define jonjext_pdo_sqlite_ptr &pdo_sqlite_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_MINIT_FUNCTION(pdo_sqlite);
JONJ_MSHUTDOWN_FUNCTION(pdo_sqlite);
JONJ_RINIT_FUNCTION(pdo_sqlite);
JONJ_RSHUTDOWN_FUNCTION(pdo_sqlite);
JONJ_MINFO_FUNCTION(pdo_sqlite);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(pdo_sqlite)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(pdo_sqlite)
*/

/* In every utility function you add that needs to use variables 
   in jonj_pdo_sqlite_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PDO_SQLITE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other jonj module directory.
*/

#ifdef ZTS
#define PDO_SQLITE_G(v) TSRMG(pdo_sqlite_globals_id, zend_pdo_sqlite_globals *, v)
#else
#define PDO_SQLITE_G(v) (pdo_sqlite_globals.v)
#endif

#endif	/* JONJ_PDO_SQLITE_H */

