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
   | Authors: George Schlossnagle <george@omniti.com>                     |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_REFLECTION_H
#define JONJ_REFLECTION_H

#include "jonj.h"

extern zend_module_entry reflection_module_entry;
#define jonjext_reflection_ptr &reflection_module_entry

BEGIN_EXTERN_C()

/* Class entry pointers */
extern JONJAPI zend_class_entry *reflector_ptr;
extern JONJAPI zend_class_entry *reflection_exception_ptr;
extern JONJAPI zend_class_entry *reflection_ptr;
extern JONJAPI zend_class_entry *reflection_function_abstract_ptr;
extern JONJAPI zend_class_entry *reflection_function_ptr;
extern JONJAPI zend_class_entry *reflection_parameter_ptr;
extern JONJAPI zend_class_entry *reflection_class_ptr;
extern JONJAPI zend_class_entry *reflection_object_ptr;
extern JONJAPI zend_class_entry *reflection_method_ptr;
extern JONJAPI zend_class_entry *reflection_property_ptr;
extern JONJAPI zend_class_entry *reflection_extension_ptr;
extern JONJAPI zend_class_entry *reflection_zend_extension_ptr;

JONJAPI void zend_reflection_class_factory(zend_class_entry *ce, zval *object TSRMLS_DC);
	
END_EXTERN_C()

#endif /* JONJ_REFLECTION_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
