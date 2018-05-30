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

#ifndef TRANSLITERATOR_METHODS_H
#define TRANSLITERATOR_METHODS_H

#include <jonj.h>

JONJ_FUNCTION( transliterator_create );

JONJ_FUNCTION( transliterator_create_from_rules );

JONJ_FUNCTION( transliterator_list_ids );

JONJ_FUNCTION( transliterator_create_inverse );

JONJ_FUNCTION( transliterator_transliterate );

JONJ_METHOD( Transliterator, __construct );

JONJ_FUNCTION( transliterator_get_error_code );

JONJ_FUNCTION( transliterator_get_error_message );

#endif /* #ifndef TRANSLITERATOR_METHODS_H */
