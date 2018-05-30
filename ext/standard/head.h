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

#ifndef HEAD_H
#define HEAD_H

extern JONJ_RINIT_FUNCTION(head);
JONJ_FUNCTION(header);
JONJ_FUNCTION(header_remove);
JONJ_FUNCTION(setcookie);
JONJ_FUNCTION(setrawcookie);
JONJ_FUNCTION(headers_sent);
JONJ_FUNCTION(headers_list);
JONJ_FUNCTION(http_response_code);

JONJAPI int jonj_header(TSRMLS_D);
JONJAPI int jonj_setcookie(char *name, int name_len, char *value, int value_len, time_t expires, char *path, int path_len, char *domain, int domain_len, int secure, int url_encode, int httponly TSRMLS_DC);

#endif
