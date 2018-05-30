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
#include "jonj_pdo_oci.h"
#include "jonj_pdo_oci_int.h"

/* {{{ pdo_oci_functions[] */
const zend_function_entry pdo_oci_functions[] = {
	JONJ_FE_END
};
/* }}} */

/* {{{ pdo_oci_module_entry */

#if ZEND_MODULE_API_NO >= 20050922
static const zend_module_dep pdo_oci_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
#endif

zend_module_entry pdo_oci_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_oci_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
	"PDO_OCI",
	pdo_oci_functions,
	JONJ_MINIT(pdo_oci),
	JONJ_MSHUTDOWN(pdo_oci),
	NULL,
	NULL,
	JONJ_MINFO(pdo_oci),
	"1.0.1",
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_OCI
ZEND_GET_MODULE(pdo_oci)
#endif

const ub4 PDO_OCI_INIT_MODE = 
#if 0 && defined(OCI_SHARED)
			/* shared mode is known to be bad for JONJ */
			OCI_SHARED
#else
			OCI_DEFAULT
#endif
#ifdef OCI_OBJECT
			|OCI_OBJECT
#endif
#ifdef ZTS
			|OCI_THREADED
#endif
			;

/* true global environment */
OCIEnv *pdo_oci_Env = NULL;

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(pdo_oci)
{
	jonj_pdo_register_driver(&pdo_oci_driver);

#if HAVE_OCIENVCREATE
	OCIEnvCreate(&pdo_oci_Env, PDO_OCI_INIT_MODE, NULL, NULL, NULL, NULL, 0, NULL);
#else
	OCIInitialize(PDO_OCI_INIT_MODE, NULL, NULL, NULL, NULL);
	OCIEnvInit(&pdo_oci_Env, OCI_DEFAULT, 0, NULL);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(pdo_oci)
{
	jonj_pdo_unregister_driver(&pdo_oci_driver);
	OCIHandleFree((dvoid*)pdo_oci_Env, OCI_HTYPE_ENV);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(pdo_oci)
{
	jonj_info_print_table_start();
	jonj_info_print_table_header(2, "PDO Driver for OCI 8 and later", "enabled");
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
