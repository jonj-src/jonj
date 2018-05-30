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

#ifndef XSL_FE_H
#define XSL_FE_H

extern const zend_function_entry jonj_xsl_xsltprocessor_class_functions[];
extern zend_class_entry *xsl_xsltprocessor_class_entry;

JONJ_FUNCTION(xsl_xsltprocessor_import_stylesheet);
JONJ_FUNCTION(xsl_xsltprocessor_transform_to_doc);
JONJ_FUNCTION(xsl_xsltprocessor_transform_to_uri);
JONJ_FUNCTION(xsl_xsltprocessor_transform_to_xml);
JONJ_FUNCTION(xsl_xsltprocessor_set_parameter);
JONJ_FUNCTION(xsl_xsltprocessor_get_parameter);
JONJ_FUNCTION(xsl_xsltprocessor_remove_parameter);
JONJ_FUNCTION(xsl_xsltprocessor_has_exslt_support);
JONJ_FUNCTION(xsl_xsltprocessor_register_jonj_functions);
JONJ_FUNCTION(xsl_xsltprocessor_set_profiling);
JONJ_FUNCTION(xsl_xsltprocessor_set_security_prefs);
JONJ_FUNCTION(xsl_xsltprocessor_get_security_prefs);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
