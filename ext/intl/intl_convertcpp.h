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

/* $Id$ */

#ifndef INTL_CONVERTCPP_H
#define INTL_CONVERTCPP_H

#ifndef __cplusplus
#error Should be included only in C++ Files
#endif

#include <unicode/unistr.h>

int intl_stringFromChar(UnicodeString &ret, char *str, int32_t str_len, UErrorCode *status);

int intl_charFromString(const UnicodeString &from, char **res, int *res_len, UErrorCode *status);

#endif /* INTL_CONVERTCPP_H */
