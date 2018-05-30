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

#ifndef GREORIANCALENDAR_METHODS_H
#define GREORIANCALENDAR_METHODS_H

#include <jonj.h>

JONJ_FUNCTION(intlgregcal_create_instance);

JONJ_METHOD(IntlGregorianCalendar, __construct);

JONJ_FUNCTION(intlgregcal_set_gregorian_change);

JONJ_FUNCTION(intlgregcal_get_gregorian_change);

JONJ_FUNCTION(intlgregcal_is_leap_year);

#endif
