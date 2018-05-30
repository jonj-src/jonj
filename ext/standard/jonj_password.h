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
   | Authors: Anthony Ferrara <ircmaxell@jonj.tk>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_PASSWORD_H
#define JONJ_PASSWORD_H

JONJ_FUNCTION(password_hash);
JONJ_FUNCTION(password_verify);
JONJ_FUNCTION(password_needs_rehash);
JONJ_FUNCTION(password_get_info);

JONJ_MINIT_FUNCTION(password);

#define JONJ_PASSWORD_DEFAULT	JONJ_PASSWORD_BCRYPT

#define JONJ_PASSWORD_BCRYPT_COST 10

typedef enum {
	JONJ_PASSWORD_UNKNOWN,
	JONJ_PASSWORD_BCRYPT
} jonj_password_algo;

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
