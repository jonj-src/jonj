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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef FORMATTER_ATTR_H
#define FORMATTER_ATTR_H

#include <jonj.h>

JONJ_FUNCTION( numfmt_set_attribute ); 
JONJ_FUNCTION( numfmt_get_attribute );
JONJ_FUNCTION( numfmt_set_text_attribute );
JONJ_FUNCTION( numfmt_get_text_attribute );
JONJ_FUNCTION( numfmt_set_symbol );
JONJ_FUNCTION( numfmt_get_symbol );
JONJ_FUNCTION( numfmt_set_pattern );
JONJ_FUNCTION( numfmt_get_pattern );
JONJ_FUNCTION( numfmt_get_locale );

#endif // FORMATTER_ATTR_H
