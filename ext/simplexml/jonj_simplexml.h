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
  | Author: Sterling Hughes <sterling@jonj.tk>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SIMPLEXML_H
#define JONJ_SIMPLEXML_H

extern zend_module_entry simplexml_module_entry;
#define jonjext_simplexml_ptr &simplexml_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include "ext/libxml/jonj_libxml.h"
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xmlerror.h>
#include <libxml/xinclude.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xpointer.h>
#include <libxml/xmlschemas.h>

JONJ_MINIT_FUNCTION(simplexml);
JONJ_MSHUTDOWN_FUNCTION(simplexml);
#ifdef HAVE_SPL
JONJ_RINIT_FUNCTION(simplexml);
#endif
JONJ_MINFO_FUNCTION(simplexml);

typedef enum {
	SXE_ITER_NONE     = 0,
	SXE_ITER_ELEMENT  = 1,
	SXE_ITER_CHILD    = 2,
	SXE_ITER_ATTRLIST = 3
} SXE_ITER;

typedef struct {
	zend_object zo;
	jonj_libxml_node_ptr *node;
	jonj_libxml_ref_obj *document;
	HashTable *properties;
	xmlXPathContextPtr xpath;
	struct {
		xmlChar               *name;
		xmlChar               *nsprefix;
		int                   isprefix;
		SXE_ITER              type;
		zval                  *data;
	} iter;
	zval *tmp;
	zend_function *fptr_count;
} jonj_sxe_object;

#ifdef ZTS
#define SIMPLEXML_G(v) TSRMG(simplexml_globals_id, zend_simplexml_globals *, v)
#else
#define SIMPLEXML_G(v) (simplexml_globals.v)
#endif

#ifdef JONJ_WIN32
#	ifdef JONJ_SIMPLEXML_EXPORTS
#		define JONJ_SXE_API __declspec(dllexport)
#	else
#		define JONJ_SXE_API __declspec(dllimport)
#	endif
#else
#	define JONJ_SXE_API ZEND_API
#endif

JONJ_SXE_API zend_class_entry *sxe_get_element_class_entry();

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
