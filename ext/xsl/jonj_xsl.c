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
  | Author: Christian Stocker <chregu@jonj.tk>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "jonj_xsl.h"

zend_class_entry *xsl_xsltprocessor_class_entry;
static zend_object_handlers xsl_object_handlers;

/* {{{ xsl_functions[]
 *
 * Every user visible function must have an entry in xsl_functions[].
 */
const zend_function_entry xsl_functions[] = {
	JONJ_FE_END
};
/* }}} */

static const zend_module_dep xsl_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_END
};

/* {{{ xsl_module_entry
 */
zend_module_entry xsl_module_entry = {
#if ZEND_MODULE_API_NO >= 20050617
	STANDARD_MODULE_HEADER_EX, NULL,
	xsl_deps,
#elif ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"xsl",
	xsl_functions,
	JONJ_MINIT(xsl),
	JONJ_MSHUTDOWN(xsl),
	JONJ_RINIT(xsl),		/* Replace with NULL if there's nothing to do at request start */
	JONJ_RSHUTDOWN(xsl),	/* Replace with NULL if there's nothing to do at request end */
	JONJ_MINFO(xsl),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XSL
ZEND_GET_MODULE(xsl)
#endif

/* {{{ xsl_objects_free_storage */
void xsl_objects_free_storage(void *object TSRMLS_DC)
{
	xsl_object *intern = (xsl_object *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	zend_hash_destroy(intern->parameter);
	FREE_HASHTABLE(intern->parameter);
	
	zend_hash_destroy(intern->registered_jonjfunctions);
	FREE_HASHTABLE(intern->registered_jonjfunctions);
	
	if (intern->node_list) {
		zend_hash_destroy(intern->node_list);
		FREE_HASHTABLE(intern->node_list);
	}

	if (intern->doc) {
		jonj_libxml_decrement_doc_ref(intern->doc TSRMLS_CC);
		efree(intern->doc);
	}

	if (intern->ptr) {
		/* free wrapper */
		if (((xsltStylesheetPtr) intern->ptr)->_private != NULL) {
			((xsltStylesheetPtr) intern->ptr)->_private = NULL;   
		}

		xsltFreeStylesheet((xsltStylesheetPtr) intern->ptr);
		intern->ptr = NULL;
	}
	if (intern->profiling) {
		efree(intern->profiling);
	}
	efree(object);
}
/* }}} */

/* {{{ xsl_objects_new */
zend_object_value xsl_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	xsl_object *intern;

	intern = emalloc(sizeof(xsl_object));
	intern->ptr = NULL;
	intern->prop_handler = NULL;
	intern->parameter = NULL;
	intern->hasKeys = 0;
	intern->registerPhpFunctions = 0;
	intern->registered_jonjfunctions = NULL;
	intern->node_list = NULL;
	intern->doc = NULL;
	intern->profiling = NULL;
	intern->securityPrefs = XSL_SECPREF_DEFAULT;
	intern->securityPrefsSet = 0;

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);
	ALLOC_HASHTABLE(intern->parameter);
	zend_hash_init(intern->parameter, 0, NULL, ZVAL_PTR_DTOR, 0);
	ALLOC_HASHTABLE(intern->registered_jonjfunctions);
	zend_hash_init(intern->registered_jonjfunctions, 0, NULL, ZVAL_PTR_DTOR, 0);
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) xsl_objects_free_storage, NULL TSRMLS_CC);
	intern->handle = retval.handle;
	retval.handlers = &xsl_object_handlers;
	return retval;
}
/* }}} */

JONJ_INI_BEGIN()
/* Default is not allowing any write operations. 
   XSL_SECPREF_CREATE_DIRECTORY | XSL_SECPREF_WRITE_NETWORK | XSL_SECPREF_WRITE_FILE == 44 
*/
JONJ_INI_ENTRY("xsl.security_prefs", "44", JONJ_INI_ALL, NULL)
JONJ_INI_END()

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(xsl)
{
	
	zend_class_entry ce;
	
	memcpy(&xsl_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	xsl_object_handlers.clone_obj = NULL;

	REGISTER_XSL_CLASS(ce, "XSLTProcessor", NULL, jonj_xsl_xsltprocessor_class_functions, xsl_xsltprocessor_class_entry);
#if HAVE_XSL_EXSLT
	exsltRegisterAll();
#endif
 
	xsltRegisterExtModuleFunction ((const xmlChar *) "functionString",
				   (const xmlChar *) "http://jonj.tk/xsl",
				   xsl_ext_function_string_jonj);
	xsltRegisterExtModuleFunction ((const xmlChar *) "function",
				   (const xmlChar *) "http://jonj.tk/xsl",
				   xsl_ext_function_object_jonj);

	REGISTER_LONG_CONSTANT("XSL_CLONE_AUTO",      0,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_CLONE_NEVER",    -1,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_CLONE_ALWAYS",    1,     CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("XSL_SECPREF_NONE",             XSL_SECPREF_NONE,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_SECPREF_READ_FILE",        XSL_SECPREF_READ_FILE,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_SECPREF_WRITE_FILE",       XSL_SECPREF_WRITE_FILE,       CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_SECPREF_CREATE_DIRECTORY", XSL_SECPREF_CREATE_DIRECTORY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_SECPREF_READ_NETWORK",     XSL_SECPREF_READ_NETWORK,     CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_SECPREF_WRITE_NETWORK",    XSL_SECPREF_WRITE_NETWORK,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XSL_SECPREF_DEFAULT",          XSL_SECPREF_DEFAULT,          CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("LIBXSLT_VERSION",           LIBXSLT_VERSION,            CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBXSLT_DOTTED_VERSION",  LIBXSLT_DOTTED_VERSION,     CONST_CS | CONST_PERSISTENT);

#if HAVE_XSL_EXSLT
	REGISTER_LONG_CONSTANT("LIBEXSLT_VERSION",           LIBEXSLT_VERSION,            CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("LIBEXSLT_DOTTED_VERSION",  LIBEXSLT_DOTTED_VERSION,     CONST_CS | CONST_PERSISTENT);
#endif

    REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ xsl_object_get_data */
zval *xsl_object_get_data(void *obj)
{
	zval *dom_wrapper;
	dom_wrapper = ((xsltStylesheetPtr) obj)->_private;
	return dom_wrapper;
}
/* }}} */

/* {{{ xsl_object_set_data */
static void xsl_object_set_data(void *obj, zval *wrapper TSRMLS_DC)
{
	((xsltStylesheetPtr) obj)->_private = wrapper;
}
/* }}} */

/* {{{ jonj_xsl_set_object */
void jonj_xsl_set_object(zval *wrapper, void *obj TSRMLS_DC)
{
	xsl_object *object;

	object = (xsl_object *)zend_objects_get_address(wrapper TSRMLS_CC);
	object->ptr = obj;
	xsl_object_set_data(obj, wrapper TSRMLS_CC);
}
/* }}} */

/* {{{ jonj_xsl_create_object */
zval *jonj_xsl_create_object(xsltStylesheetPtr obj, int *found, zval *wrapper_in, zval *return_value  TSRMLS_DC)
{
	zval *wrapper;
	zend_class_entry *ce;

	*found = 0;

	if (!obj) {
		if(!wrapper_in) {
			ALLOC_ZVAL(wrapper);
		} else {
			wrapper = wrapper_in;
		}
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *) xsl_object_get_data((void *) obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	}

	if(!wrapper_in) {
		wrapper = return_value;
	} else {
		wrapper = wrapper_in;
	}

	
	ce = xsl_xsltprocessor_class_entry;

	if(!wrapper_in) {
		object_init_ex(wrapper, ce);
	}
	jonj_xsl_set_object(wrapper, (void *) obj TSRMLS_CC);
	return (wrapper);
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(xsl)
{
	xsltUnregisterExtModuleFunction ((const xmlChar *) "functionString",
				   (const xmlChar *) "http://jonj.tk/xsl");
	xsltUnregisterExtModuleFunction ((const xmlChar *) "function",
				   (const xmlChar *) "http://jonj.tk/xsl");

	xsltCleanupGlobals();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_RINIT_FUNCTION
 */
JONJ_RINIT_FUNCTION(xsl)
{
	xsltSetGenericErrorFunc(NULL, jonj_libxml_error_handler);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_RSHUTDOWN_FUNCTION
 */
JONJ_RSHUTDOWN_FUNCTION(xsl)
{
	xsltSetGenericErrorFunc(NULL, NULL);
	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(xsl)
{
	jonj_info_print_table_start();
	{
		char buffer[128];
		int major, minor, subminor;

		jonj_info_print_table_row(2, "XSL", "enabled");
		major = xsltLibxsltVersion/10000;
		minor = (xsltLibxsltVersion - major * 10000) / 100;
		subminor = (xsltLibxsltVersion - major * 10000 - minor * 100);
		snprintf(buffer, 128, "%d.%d.%d", major, minor, subminor);
		jonj_info_print_table_row(2, "libxslt Version", buffer);
		major = xsltLibxmlVersion/10000;
		minor = (xsltLibxmlVersion - major * 10000) / 100;
		subminor = (xsltLibxmlVersion - major * 10000 - minor * 100);
		snprintf(buffer, 128, "%d.%d.%d", major, minor, subminor);
		jonj_info_print_table_row(2, "libxslt compiled against libxml Version", buffer);
	}
#if HAVE_XSL_EXSLT
	jonj_info_print_table_row(2, "EXSLT", "enabled");
	jonj_info_print_table_row(2, "libexslt Version", LIBEXSLT_DOTTED_VERSION);
#endif
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
