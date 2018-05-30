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
  |         Marcus Boerger <helly@jonj.tk>                               |
  |         Rob Richards <rrichards@jonj.tk>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SIMPLEXML_EXPORTS_H
#define JONJ_SIMPLEXML_EXPORTS_H

#include "jonj_simplexml.h"

#define SKIP_TEXT(__p) \
	if ((__p)->type == XML_TEXT_NODE) { \
		goto next_iter; \
	}

#define GET_NODE(__s, __n) { \
	if ((__s)->node && (__s)->node->node) { \
		__n = (__s)->node->node; \
	} else { \
		__n = NULL; \
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Node no longer exists"); \
	} \
}

JONJ_SXE_API zend_object_value sxe_object_new(zend_class_entry *ce TSRMLS_DC);
/* {{{ jonj_sxe_fetch_object()
 */
static inline jonj_sxe_object *
jonj_sxe_fetch_object(zval *object TSRMLS_DC)
{
	return (jonj_sxe_object *) zend_object_store_get_object(object TSRMLS_CC);
}
/* }}} */

typedef struct {
	zend_object_iterator  intern;
	jonj_sxe_object        *sxe;
} jonj_sxe_iterator;

#endif /* JONJ_SIMPLEXML_EXPORTS_H */

/**
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
