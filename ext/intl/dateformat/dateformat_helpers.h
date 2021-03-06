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

#ifndef DATEFORMAT_HELPERS_H
#define	DATEFORMAT_HELPERS_H

#ifndef __cplusplus
#error For C++ only
#endif

#include <unicode/calendar.h>

extern "C" {
#include "../jonj_intl.h"
}

int datefmt_process_calendar_arg(zval* calendar_zv,
								 Locale const& locale,
								 const char *func_name,
								 intl_error *err,
								 Calendar*& cal,
								 long& cal_int_type,
								 bool& calendar_owned TSRMLS_DC);

#endif	/* DATEFORMAT_HELPERS_H */

