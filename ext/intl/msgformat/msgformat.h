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

#ifndef MSG_FORMATTER_H
#define MSG_FORMATTER_H

#include <jonj.h>

JONJ_FUNCTION( msgfmt_create );
JONJ_FUNCTION( msgfmt_get_error_code );
JONJ_FUNCTION( msgfmt_get_error_message );
JONJ_METHOD( MessageFormatter, __construct );

#endif // MSG_FORMAT_H
