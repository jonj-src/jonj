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
   | Authors: Stig Bakken <ssb@jonj.tk>                                   |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@jonj.tk>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_CRYPT_H
#define JONJ_CRYPT_H

JONJAPI int jonj_crypt(const char *password, const int pass_len, const char *salt, int salt_len, char **result);
JONJ_FUNCTION(crypt);
#if HAVE_CRYPT
JONJ_MINIT_FUNCTION(crypt);
JONJ_MSHUTDOWN_FUNCTION(crypt);
JONJ_RINIT_FUNCTION(crypt);
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
