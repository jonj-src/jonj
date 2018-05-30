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
   | Authors: Gustavo Lopes <cataphract@netcabo.ot>                       |
   +----------------------------------------------------------------------+
 */

#ifndef TRANSLITERATOR_TRANSLITERATOR_H
#define TRANSLITERATOR_TRANSLITERATOR_H

#include <jonj.h>
#include <unicode/utypes.h>
#include <unicode/utrans.h>

#include "ext/standard/jonj_smart_str.h"

void transliterator_register_constants( INIT_FUNC_ARGS );
smart_str transliterator_parse_error_to_string( UParseError* pe );

#endif /* #ifndef TRANSLITERATOR_TRANSLITERATOR_H */
