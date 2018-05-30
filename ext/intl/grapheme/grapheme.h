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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#ifndef GRAPHEME_GRAPHEME_H
#define GRAPHEME_GRAPHEME_H

#include <jonj.h>
#include <unicode/utypes.h>

JONJ_FUNCTION(grapheme_strlen);
JONJ_FUNCTION(grapheme_strpos);
JONJ_FUNCTION(grapheme_stripos);
JONJ_FUNCTION(grapheme_strrpos);
JONJ_FUNCTION(grapheme_strripos);
JONJ_FUNCTION(grapheme_substr);
JONJ_FUNCTION(grapheme_strstr);
JONJ_FUNCTION(grapheme_stristr);
JONJ_FUNCTION(grapheme_extract);

void grapheme_register_constants( INIT_FUNC_ARGS );
void grapheme_close_global_iterator( TSRMLS_D );

#endif // GRAPHEME_GRAPHEME_H
