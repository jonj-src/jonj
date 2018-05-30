/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Hans-Peter Oeri (University of St.Gallen) <hp@oeri.ch>      |
   +----------------------------------------------------------------------+
 */

#ifndef RESOURCEBUNDLE_CLASS_H
#define RESOURCEBUNDLE_CLASS_H

#include <unicode/ures.h>

#include <zend.h>
#include "jonj.h"

#include "intl_error.h"

typedef struct {
	zend_object     zend;
	intl_error      error;

	UResourceBundle *me;
	UResourceBundle *child;
} ResourceBundle_object;

#define RESOURCEBUNDLE_METHOD_INIT_VARS		INTL_METHOD_INIT_VARS(ResourceBundle, rb)
#define RESOURCEBUNDLE_METHOD_FETCH_OBJECT_NO_CHECK	INTL_METHOD_FETCH_OBJECT(ResourceBundle, rb)
#define RESOURCEBUNDLE_METHOD_FETCH_OBJECT							\
	INTL_METHOD_FETCH_OBJECT(ResourceBundle, rb);					\
	if (RESOURCEBUNDLE_OBJECT(rb) == NULL) {						\
		intl_errors_set(&rb->error, U_ILLEGAL_ARGUMENT_ERROR,		\
				"Found unconstructed ResourceBundle", 0 TSRMLS_CC);	\
		RETURN_FALSE;												\
	}


#define RESOURCEBUNDLE_OBJECT(rb)			(rb)->me

void resourcebundle_register_class( TSRMLS_D );
extern zend_class_entry *ResourceBundle_ce_ptr;

JONJ_FUNCTION( resourcebundle_create );
JONJ_FUNCTION( resourcebundle_get );
JONJ_FUNCTION( resourcebundle_count );
JONJ_FUNCTION( resourcebundle_locales );
JONJ_FUNCTION( resourcebundle_get_error_code );
JONJ_FUNCTION( resourcebundle_get_error_message );

#endif // #ifndef RESOURCEBUNDLE_CLASS_H
