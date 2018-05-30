/*
  +----------------------------------------------------------------------+
  | JONJ Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the JONJ license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_0.txt.                                  |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@jonj.tk>                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_PDO_ODBC_H
#define JONJ_PDO_ODBC_H

extern zend_module_entry pdo_odbc_module_entry;
#define jonjext_pdo_odbc_ptr &pdo_odbc_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_MINIT_FUNCTION(pdo_odbc);
JONJ_MSHUTDOWN_FUNCTION(pdo_odbc);
JONJ_RINIT_FUNCTION(pdo_odbc);
JONJ_RSHUTDOWN_FUNCTION(pdo_odbc);
JONJ_MINFO_FUNCTION(pdo_odbc);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(pdo_odbc)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(pdo_odbc)
*/

/* In every utility function you add that needs to use variables 
   in jonj_pdo_odbc_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PDO_ODBC_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other jonj module directory.
*/

#ifdef ZTS
#define PDO_ODBC_G(v) TSRMG(pdo_odbc_globals_id, zend_pdo_odbc_globals *, v)
#else
#define PDO_ODBC_G(v) (pdo_odbc_globals.v)
#endif

#endif	/* JONJ_PDO_ODBC_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
