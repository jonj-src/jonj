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
   | Author: Kirill Maximov (kir@rus.net)                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef QUOT_PRINT_H
#define QUOT_PRINT_H

JONJAPI unsigned char *jonj_quot_print_decode(const unsigned char *str, size_t length, size_t *ret_length, int replace_us_by_ws);
JONJAPI unsigned char *jonj_quot_print_encode(const unsigned char *str, size_t length, size_t *ret_length);

JONJ_FUNCTION(quoted_printable_decode);
JONJ_FUNCTION(quoted_printable_encode);

#endif /* QUOT_PRINT_H */
