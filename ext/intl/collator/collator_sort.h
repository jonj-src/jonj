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
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifndef COLLATOR_SORT_H
#define COLLATOR_SORT_H

#include <jonj.h>

typedef int (*collator_compare_func_t)( zval *result, zval *op1, zval *op2 TSRMLS_DC );

JONJ_FUNCTION( collator_sort );
JONJ_FUNCTION( collator_sort_with_sort_keys );
JONJ_FUNCTION( collator_get_sort_key );
JONJ_FUNCTION( collator_asort );

#endif // COLLATOR_SORT_H
