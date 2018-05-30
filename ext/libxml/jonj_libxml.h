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
   | Authors: Shane Caraveo <shane@jonj.tk>                               |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_LIBXML_H
#define JONJ_LIBXML_H

#if HAVE_LIBXML
extern zend_module_entry libxml_module_entry;
#define libxml_module_ptr &libxml_module_entry

#ifdef JONJ_WIN32
#	define JONJ_LIBXML_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define JONJ_LIBXML_API __attribute__ ((visibility("default")))
#else
#	define JONJ_LIBXML_API
#endif

#include "ext/standard/jonj_smart_str.h"
#include <libxml/tree.h>

#define LIBXML_SAVE_NOEMPTYTAG 1<<2

ZEND_BEGIN_MODULE_GLOBALS(libxml)
	zval *stream_context;
	smart_str error_buffer;
	zend_llist *error_list;
	struct _jonj_libxml_entity_resolver {
		zend_fcall_info			fci;
		zend_fcall_info_cache	fcc;
	} entity_loader;
	zend_bool entity_loader_disabled;
ZEND_END_MODULE_GLOBALS(libxml)

typedef struct _libxml_doc_props {
	int formatoutput;
	int validateonparse;
	int resolveexternals;
	int preservewhitespace;
	int substituteentities;
	int stricterror;
	int recover;
	HashTable *classmap;
} libxml_doc_props;

typedef struct _jonj_libxml_ref_obj {
	void *ptr;
	int   refcount;
	libxml_doc_props *doc_props;
} jonj_libxml_ref_obj;

typedef struct _jonj_libxml_node_ptr {
	xmlNodePtr node;
	int	refcount;
	void *_private;
} jonj_libxml_node_ptr;

typedef struct _jonj_libxml_node_object {
	zend_object  std;
	jonj_libxml_node_ptr *node;
	jonj_libxml_ref_obj *document;
	HashTable *properties;
} jonj_libxml_node_object;

typedef void * (*jonj_libxml_export_node) (zval *object TSRMLS_DC);

JONJ_LIBXML_API int jonj_libxml_increment_node_ptr(jonj_libxml_node_object *object, xmlNodePtr node, void *private_data TSRMLS_DC);
JONJ_LIBXML_API int jonj_libxml_decrement_node_ptr(jonj_libxml_node_object *object TSRMLS_DC);
JONJ_LIBXML_API int jonj_libxml_increment_doc_ref(jonj_libxml_node_object *object, xmlDocPtr docp TSRMLS_DC);
JONJ_LIBXML_API int jonj_libxml_decrement_doc_ref(jonj_libxml_node_object *object TSRMLS_DC);
JONJ_LIBXML_API xmlNodePtr jonj_libxml_import_node(zval *object TSRMLS_DC);
JONJ_LIBXML_API int jonj_libxml_register_export(zend_class_entry *ce, jonj_libxml_export_node export_function);
/* When an explicit freeing of node and children is required */
JONJ_LIBXML_API void jonj_libxml_node_free_resource(xmlNodePtr node TSRMLS_DC);
/* When object dtor is called as node may still be referenced */
JONJ_LIBXML_API void jonj_libxml_node_decrement_resource(jonj_libxml_node_object *object TSRMLS_DC);
JONJ_LIBXML_API void jonj_libxml_error_handler(void *ctx, const char *msg, ...);
JONJ_LIBXML_API void jonj_libxml_ctx_warning(void *ctx, const char *msg, ...);
JONJ_LIBXML_API void jonj_libxml_ctx_error(void *ctx, const char *msg, ...);
JONJ_LIBXML_API int jonj_libxml_xmlCheckUTF8(const unsigned char *s);
JONJ_LIBXML_API zval *jonj_libxml_switch_context(zval *context TSRMLS_DC);
JONJ_LIBXML_API void jonj_libxml_issue_error(int level, const char *msg TSRMLS_DC);
JONJ_LIBXML_API zend_bool jonj_libxml_disable_entity_loader(zend_bool disable TSRMLS_DC);

/* Init/shutdown functions*/
JONJ_LIBXML_API void jonj_libxml_initialize(void);
JONJ_LIBXML_API void jonj_libxml_shutdown(void);

#ifdef ZTS
#define LIBXML(v) TSRMG(libxml_globals_id, zend_libxml_globals *, v)
#else
#define LIBXML(v) (libxml_globals.v)
#endif

#else /* HAVE_LIBXML */
#define libxml_module_ptr NULL
#endif

#define jonjext_libxml_ptr libxml_module_ptr

#endif /* JONJ_LIBXML_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
