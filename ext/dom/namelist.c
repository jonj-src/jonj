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
   | Authors: Christian Stocker <chregu@jonj.tk>                          |
   |          Rob Richards <rrichards@jonj.tk>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#if HAVE_LIBXML && HAVE_DOM
#include "jonj_dom.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_namelist_get_name, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_namelist_get_namespace_uri, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMNameList 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList
* Since: DOM Level 3
*/

const zend_function_entry jonj_dom_namelist_class_functions[] = {
	JONJ_FALIAS(getName, dom_namelist_get_name, arginfo_dom_namelist_get_name)
	JONJ_FALIAS(getNamespaceURI, dom_namelist_get_namespace_uri, arginfo_dom_namelist_get_namespace_uri)
	JONJ_FE_END
};

/* {{{ length	int	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList-length
Since: 
*/
int dom_namelist_length_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, "TEST", 1);
	return SUCCESS;
}

/* }}} */

/* {{{ proto string dom_namelist_get_name(int index);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList-getName
Since: 
*/
JONJ_FUNCTION(dom_namelist_get_name)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_namelist_get_name */

/* {{{ proto string dom_namelist_get_namespace_uri(int index);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#NameList-getNamespaceURI
Since: 
*/
JONJ_FUNCTION(dom_namelist_get_namespace_uri)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_namelist_get_namespace_uri */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
