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
  | Author: Ard Biesheuvel <abies@jonj.tk>                               |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "pdo/jonj_pdo.h"
#include "pdo/jonj_pdo_driver.h"
#include "jonj_pdo_firebird.h"
#include "jonj_pdo_firebird_int.h"

const zend_function_entry pdo_firebird_functions[] = { /* {{{ */
	JONJ_FE_END
};
/* }}} */

/* {{{ pdo_firebird_deps
 */
static const zend_module_dep pdo_firebird_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
/* }}} */

zend_module_entry pdo_firebird_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_firebird_deps,
	"PDO_Firebird",
	pdo_firebird_functions,
	JONJ_MINIT(pdo_firebird),
	JONJ_MSHUTDOWN(pdo_firebird),
	NULL,
	NULL,
	JONJ_MINFO(pdo_firebird),
	"0.3",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_FIREBIRD
ZEND_GET_MODULE(pdo_firebird)
#endif

JONJ_MINIT_FUNCTION(pdo_firebird) /* {{{ */
{
	REGISTER_PDO_CLASS_CONST_LONG("FB_ATTR_DATE_FORMAT", (long) PDO_FB_ATTR_DATE_FORMAT);
	REGISTER_PDO_CLASS_CONST_LONG("FB_ATTR_TIME_FORMAT", (long) PDO_FB_ATTR_TIME_FORMAT);
	REGISTER_PDO_CLASS_CONST_LONG("FB_ATTR_TIMESTAMP_FORMAT", (long) PDO_FB_ATTR_TIMESTAMP_FORMAT);

	jonj_pdo_register_driver(&pdo_firebird_driver);

	return SUCCESS;
}
/* }}} */

JONJ_MSHUTDOWN_FUNCTION(pdo_firebird) /* {{{ */
{
	jonj_pdo_unregister_driver(&pdo_firebird_driver);

	return SUCCESS;
}
/* }}} */

JONJ_MINFO_FUNCTION(pdo_firebird) /* {{{ */
{
	jonj_info_print_table_start();
	jonj_info_print_table_header(2, "PDO Driver for Firebird", "enabled");
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
