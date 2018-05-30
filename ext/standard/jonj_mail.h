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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_MAIL_H
#define JONJ_MAIL_H

JONJ_FUNCTION(mail);
JONJ_MINFO_FUNCTION(mail);

JONJ_FUNCTION(ezmlm_hash);
JONJAPI extern int jonj_mail(char *to, char *subject, char *message, char *headers, char *extra_cmd TSRMLS_DC);

#endif /* JONJ_MAIL_H */
