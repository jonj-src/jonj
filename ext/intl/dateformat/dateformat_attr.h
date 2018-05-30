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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifndef DATE_FORMAT_ATTR_H
#define DATE_FORMAT_ATTR_H

#include <jonj.h>

//JONJ_FUNCTION( datefmt_get_timezone );
JONJ_FUNCTION( datefmt_get_datetype );
JONJ_FUNCTION( datefmt_get_timetype );
JONJ_FUNCTION( datefmt_get_locale );
JONJ_FUNCTION( datefmt_get_pattern );
JONJ_FUNCTION( datefmt_set_pattern );
JONJ_FUNCTION( datefmt_is_lenient );
JONJ_FUNCTION( datefmt_set_lenient );

#endif // DATE_FORMAT_ATTR_H
