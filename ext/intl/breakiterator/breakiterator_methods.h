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

#ifndef BREAKITERATOR_METHODS_H
#define BREAKITERATOR_METHODS_H

#include <jonj.h>

JONJ_METHOD(BreakIterator, __construct);

JONJ_FUNCTION(breakiter_create_word_instance);

JONJ_FUNCTION(breakiter_create_line_instance);

JONJ_FUNCTION(breakiter_create_character_instance);

JONJ_FUNCTION(breakiter_create_sentence_instance);

JONJ_FUNCTION(breakiter_create_title_instance);

JONJ_FUNCTION(breakiter_create_code_point_instance);

JONJ_FUNCTION(breakiter_get_text);

JONJ_FUNCTION(breakiter_set_text);

JONJ_FUNCTION(breakiter_first);

JONJ_FUNCTION(breakiter_last);

JONJ_FUNCTION(breakiter_previous);

JONJ_FUNCTION(breakiter_next);

JONJ_FUNCTION(breakiter_current);

JONJ_FUNCTION(breakiter_following);

JONJ_FUNCTION(breakiter_preceding);

JONJ_FUNCTION(breakiter_is_boundary);

JONJ_FUNCTION(breakiter_get_locale);

JONJ_FUNCTION(breakiter_get_parts_iterator);

JONJ_FUNCTION(breakiter_get_error_code);

JONJ_FUNCTION(breakiter_get_error_message);

#endif
