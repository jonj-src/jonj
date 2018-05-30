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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "pdo/jonj_pdo.h"
#include "pdo/jonj_pdo_driver.h"
#include "jonj_pdo_sqlite.h"
#include "jonj_pdo_sqlite_int.h"
#include "zend_exceptions.h"

#define JONJ_PDO_SQLITE_MODULE_VERSION	"1.0.1"

/* {{{ pdo_sqlite_functions[] */
const zend_function_entry pdo_sqlite_functions[] = {
	JONJ_FE_END
};
/* }}} */

/* {{{ pdo_sqlite_deps
 */
#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_sqlite_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
#endif
/* }}} */

/* {{{ pdo_sqlite_module_entry
 */
zend_module_entry pdo_sqlite_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_sqlite_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"pdo_sqlite",
	pdo_sqlite_functions,
	JONJ_MINIT(pdo_sqlite),
	JONJ_MSHUTDOWN(pdo_sqlite),
	NULL,
	NULL,
	JONJ_MINFO(pdo_sqlite),
	JONJ_PDO_SQLITE_MODULE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if defined(COMPILE_DL_PDO_SQLITE) || defined(COMPILE_DL_PDO_SQLITE_EXTERNAL)
ZEND_GET_MODULE(pdo_sqlite)
#endif

/* {{{ JONJ_MINIT_FUNCTION */
JONJ_MINIT_FUNCTION(pdo_sqlite)
{
	return jonj_pdo_register_driver(&pdo_sqlite_driver);
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION */
JONJ_MSHUTDOWN_FUNCTION(pdo_sqlite)
{
	jonj_pdo_unregister_driver(&pdo_sqlite_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(pdo_sqlite)
{
	jonj_info_print_table_start();
	jonj_info_print_table_header(2, "PDO Driver for SQLite 3.x", "enabled");
	jonj_info_print_table_row(2, "SQLite Library", sqlite3_libversion());
	jonj_info_print_table_end();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
