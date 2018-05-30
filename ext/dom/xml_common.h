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

#ifndef JONJ_XML_COMMON_H
#define JONJ_XML_COMMON_H

#include "ext/libxml/jonj_libxml.h"

typedef libxml_doc_props *dom_doc_propsptr;

typedef struct _dom_object {
	zend_object  std;
	void *ptr;
	jonj_libxml_ref_obj *document;
	HashTable *prop_handler;
	zend_object_handle handle;
} dom_object;

#ifdef JONJ_WIN32
#	ifdef JONJAPI
#		undef JONJAPI
#	endif
#	ifdef DOM_EXPORTS
#		define JONJAPI __declspec(dllexport)
#	else
#		define JONJAPI __declspec(dllimport)
#	endif /* DOM_EXPORTS */
#elif defined(__GNUC__) && __GNUC__ >= 4
#	ifdef JONJAPI
#		undef JONJAPI
#	endif
#	define JONJAPI __attribute__ ((visibility("default")))
#endif /* JONJ_WIN32 */

#define JONJ_DOM_EXPORT JONJAPI

JONJ_DOM_EXPORT extern zend_class_entry *dom_node_class_entry;
JONJ_DOM_EXPORT dom_object *jonj_dom_object_get_data(xmlNodePtr obj);
JONJ_DOM_EXPORT zval *jonj_dom_create_object(xmlNodePtr obj, int *found, zval* return_value, dom_object *domobj TSRMLS_DC);
JONJ_DOM_EXPORT xmlNodePtr dom_object_get_node(dom_object *obj);

#define DOM_XMLNS_NAMESPACE \
    (const xmlChar *) "http://www.w3.org/2000/xmlns/"

#define NODE_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	__intern = (jonj_libxml_node_object *)zend_object_store_get_object(__id TSRMLS_CC); \
	if (__intern->node == NULL || !(__ptr = (__prtype)__intern->node->node)) { \
  		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", __intern->std.ce->name);\
  		RETURN_NULL();\
  	} \
}

#define DOC_GET_OBJ(__ptr, __id, __prtype, __intern) { \
	__intern = (jonj_libxml_node_object *)zend_object_store_get_object(__id TSRMLS_CC); \
	if (__intern->document != NULL) { \
		if (!(__ptr = (__prtype)__intern->document->ptr)) { \
  			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", __intern->std.ce->name);\
  			RETURN_NULL();\
  		} \
	} \
}

#define DOM_RET_OBJ(obj, ret, domobject) \
	if (!jonj_dom_create_object(obj, ret, return_value, domobject TSRMLS_CC)) { \
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object"); \
		RETURN_FALSE; \
	}

#define DOM_GET_THIS(zval) \
	if (NULL == (zval = getThis())) { \
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Underlying object missing"); \
		RETURN_FALSE; \
	}

#define DOM_GET_THIS_OBJ(__ptr, __id, __prtype, __intern) \
	DOM_GET_THIS(__id); \
	DOM_GET_OBJ(__ptr, __id, __prtype, __intern);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
