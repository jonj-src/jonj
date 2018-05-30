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
  | Author: George Schlossnagle <george@omniti.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_PDO_MYSQL_H
#define JONJ_PDO_MYSQL_H

extern zend_module_entry pdo_mysql_module_entry;
#define jonjext_pdo_mysql_ptr &pdo_mysql_module_entry

#ifdef JONJ_WIN32
#define JONJ_PDO_MYSQL_API __declspec(dllexport)
#else
#define JONJ_PDO_MYSQL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


#endif	/* JONJ_PDO_MYSQL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
