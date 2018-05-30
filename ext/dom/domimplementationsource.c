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
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_implementationsource_getdomimplementation, 0, 0, 1)
	ZEND_ARG_INFO(0, features)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_implementationsource_getdomimplementations, 0, 0, 1)
	ZEND_ARG_INFO(0, features)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class domimplementationsource 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMImplementationSource
* Since: DOM Level 3
*/

const zend_function_entry jonj_dom_domimplementationsource_class_functions[] = {
	JONJ_FALIAS(getDomimplementation, dom_domimplementationsource_get_domimplementation, arginfo_dom_implementationsource_getdomimplementation)
	JONJ_FALIAS(getDomimplementations, dom_domimplementationsource_get_domimplementations, arginfo_dom_implementationsource_getdomimplementations)
	JONJ_FE_END
};

/* {{{ attribute protos, not implemented yet */

/* {{{ proto domdomimplementation dom_domimplementationsource_get_domimplementation(string features);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-getDOMImpl
Since: 
*/
JONJ_FUNCTION(dom_domimplementationsource_get_domimplementation)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domimplementationsource_get_domimplementation */

/* {{{ proto domimplementationlist dom_domimplementationsource_get_domimplementations(string features);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-getDOMImpls
Since: 
*/
JONJ_FUNCTION(dom_domimplementationsource_get_domimplementations)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domimplementationsource_get_domimplementations */

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
