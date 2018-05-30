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
  |         Frank M. Kromann <frank@kromann.info>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_PDO_DBLIB_H
#define JONJ_PDO_DBLIB_H

#if PDO_DBLIB_IS_MSSQL
extern zend_module_entry pdo_mssql_module_entry;
#define jonjext_pdo_mssql_ptr &pdo_mssql_module_entry
#else
extern zend_module_entry pdo_dblib_module_entry;
#define jonjext_pdo_dblib_ptr &pdo_dblib_module_entry
#endif

#ifdef ZTS
# include "TSRM.h"
#endif

JONJ_MINIT_FUNCTION(pdo_dblib);
JONJ_MSHUTDOWN_FUNCTION(pdo_dblib);
JONJ_MINFO_FUNCTION(pdo_dblib);
JONJ_RSHUTDOWN_FUNCTION(pdo_dblib);

#endif

