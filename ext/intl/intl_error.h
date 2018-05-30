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
   |          Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef INTL_ERROR_H
#define INTL_ERROR_H

#include <unicode/utypes.h>
#include <unicode/parseerr.h>
#include <ext/standard/jonj_smart_str.h>

#define INTL_ERROR_CODE(e) (e).code

typedef struct _intl_error {
	UErrorCode      code;
	char*           custom_error_message;
	int             free_custom_error_message;
} intl_error;

intl_error* intl_error_create( TSRMLS_D );
void        intl_error_init( intl_error* err TSRMLS_DC );
void        intl_error_reset( intl_error* err TSRMLS_DC );
void        intl_error_set_code( intl_error* err, UErrorCode err_code TSRMLS_DC );
void        intl_error_set_custom_msg( intl_error* err, char* msg, int copyMsg TSRMLS_DC );
void        intl_error_set( intl_error* err, UErrorCode code, char* msg, int copyMsg TSRMLS_DC );
UErrorCode  intl_error_get_code( intl_error* err TSRMLS_DC );
char*       intl_error_get_message( intl_error* err TSRMLS_DC );

// Wrappers to synchonize object's and global error structures.
void        intl_errors_reset( intl_error* err TSRMLS_DC );
void        intl_errors_set_custom_msg( intl_error* err, char* msg, int copyMsg TSRMLS_DC );
void        intl_errors_set_code( intl_error* err, UErrorCode err_code TSRMLS_DC );
void        intl_errors_set( intl_error* err, UErrorCode code, char* msg, int copyMsg TSRMLS_DC );

// Other error helpers
smart_str	intl_parse_error_to_string( UParseError* pe );

// exported to be called on extension MINIT
void		intl_register_IntlException_class( TSRMLS_D );

#endif // INTL_ERROR_H