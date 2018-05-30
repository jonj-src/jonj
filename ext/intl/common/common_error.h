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

#ifndef INTL_COMMON_ERROR_H
#define INTL_COMMON_ERROR_H

#include <jonj.h>

JONJ_FUNCTION( intl_get_error_code );
JONJ_FUNCTION( intl_get_error_message );
JONJ_FUNCTION( intl_is_failure );
JONJ_FUNCTION( intl_error_name );

void intl_expose_icu_error_codes( INIT_FUNC_ARGS );

#endif // INTL_COMMON_ERROR_H
