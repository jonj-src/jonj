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
   | Authors: Gustavo Lopes <cataphract@jonj.tk>                          |
   +----------------------------------------------------------------------+
 */

#include "codepointiterator_internal.h"

extern "C" {
#define USE_BREAKITERATOR_POINTER 1
#include "breakiterator_class.h"
}

using JONJ::CodePointBreakIterator;

static inline CodePointBreakIterator *fetch_cpbi(BreakIterator_object *bio) {
	return (CodePointBreakIterator*)bio->biter;
}

U_CFUNC JONJ_FUNCTION(cpbi_get_last_code_point)
{
	BREAKITER_METHOD_INIT_VARS;
	object = getThis();

	if (zend_parse_parameters_none() == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"cpbi_get_last_code_point: bad arguments", 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	BREAKITER_METHOD_FETCH_OBJECT;

	RETURN_LONG(fetch_cpbi(bio)->getLastCodePoint());
}
