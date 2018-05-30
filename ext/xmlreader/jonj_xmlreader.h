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
  | Author: Rob Richards <rrichards@jonj.tk>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_XMLREADER_H
#define JONJ_XMLREADER_H

extern zend_module_entry xmlreader_module_entry;
#define jonjext_xmlreader_ptr &xmlreader_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#include "ext/libxml/jonj_libxml.h"
#include <libxml/xmlreader.h>

typedef struct _xmlreader_object {
	zend_object  std;
	xmlTextReaderPtr ptr;
	/* strings must be set in input buffer as copy is required */
	xmlParserInputBufferPtr input;
	void *schema;
	HashTable *prop_handler;
	zend_object_handle handle;
} xmlreader_object;

JONJ_MINIT_FUNCTION(xmlreader);
JONJ_MSHUTDOWN_FUNCTION(xmlreader);
JONJ_MINFO_FUNCTION(xmlreader);

#define REGISTER_XMLREADER_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(xmlreader_class_entry, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

#ifdef ZTS
#define XMLREADER_G(v) TSRMG(xmlreader_globals_id, zend_xmlreader_globals *, v)
#else
#define XMLREADER_G(v) (xmlreader_globals.v)
#endif

#endif	/* JONJ_XMLREADER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
