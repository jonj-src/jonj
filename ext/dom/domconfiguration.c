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
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_configuration_set_parameter, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_configuration_get_parameter, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_configuration_can_set_parameter, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class domdomconfiguration 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration
* Since: DOM Level 3
*/

const zend_function_entry jonj_dom_domconfiguration_class_functions[] = {
	JONJ_FALIAS(setParameter, dom_domconfiguration_set_parameter, arginfo_dom_configuration_set_parameter)
	JONJ_FALIAS(getParameter, dom_domconfiguration_get_parameter, arginfo_dom_configuration_get_parameter)
	JONJ_FALIAS(canSetParameter, dom_domconfiguration_can_set_parameter, arginfo_dom_configuration_can_set_parameter)
	JONJ_FE_END
};

/* {{{ attribute protos, not implemented yet */

/* {{{ proto dom_void dom_domconfiguration_set_parameter(string name, domuserdata value);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration-property
Since: 
*/
JONJ_FUNCTION(dom_domconfiguration_set_parameter)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domconfiguration_set_parameter */

/* {{{ proto domdomuserdata dom_domconfiguration_get_parameter(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration-getParameter
Since: 
*/
JONJ_FUNCTION(dom_domconfiguration_get_parameter)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domconfiguration_get_parameter */

/* {{{ proto boolean dom_domconfiguration_can_set_parameter(string name, domuserdata value);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration-canSetParameter
Since: 
*/
JONJ_FUNCTION(dom_domconfiguration_can_set_parameter)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domconfiguration_can_set_parameter */

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
