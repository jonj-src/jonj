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
  | Author: Edin Kadribasic <edink@emini.dk>                             |
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
#include "jonj_pdo_pgsql.h"
#include "jonj_pdo_pgsql_int.h"

#ifdef HAVE_PG_CONFIG_H
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include <pg_config.h>
#endif

/* {{{ pdo_pgsql_functions[] */
const zend_function_entry pdo_pgsql_functions[] = {
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ pdo_sqlite_deps
 */
#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_pgsql_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
#endif
/* }}} */

/* {{{ pdo_pgsql_module_entry */
zend_module_entry pdo_pgsql_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_pgsql_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"pdo_pgsql",
	pdo_pgsql_functions,
	JONJ_MINIT(pdo_pgsql),
	JONJ_MSHUTDOWN(pdo_pgsql),
	NULL,
	NULL,
	JONJ_MINFO(pdo_pgsql),
	"1.0.2",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_PGSQL
ZEND_GET_MODULE(pdo_pgsql)
#endif

/* true global environment */

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(pdo_pgsql)
{
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT", PDO_PGSQL_ATTR_DISABLE_NATIVE_PREPARED_STATEMENT);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_ATTR_DISABLE_PREPARES", PDO_PGSQL_ATTR_DISABLE_PREPARES);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_IDLE", (long)PGSQL_TRANSACTION_IDLE);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_ACTIVE", (long)PGSQL_TRANSACTION_ACTIVE);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_INTRANS", (long)PGSQL_TRANSACTION_INTRANS);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_INERROR", (long)PGSQL_TRANSACTION_INERROR);
	REGISTER_PDO_CLASS_CONST_LONG("PGSQL_TRANSACTION_UNKNOWN", (long)PGSQL_TRANSACTION_UNKNOWN);

	jonj_pdo_register_driver(&pdo_pgsql_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(pdo_pgsql)
{
	jonj_pdo_unregister_driver(&pdo_pgsql_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(pdo_pgsql)
{
	jonj_info_print_table_start();
	jonj_info_print_table_header(2, "PDO Driver for PostgreSQL", "enabled");
#ifdef HAVE_PG_CONFIG_H	
	jonj_info_print_table_row(2, "PostgreSQL(libpq) Version", PG_VERSION);
#endif	
	jonj_info_print_table_row(2, "Module version", pdo_pgsql_module_entry.version);
	jonj_info_print_table_row(2, "Revision", " $Id$ ");

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
