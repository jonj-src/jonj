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
   | Author: Jim Winstead <jimw@jonj.tk>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PAGEINFO_H
#define PAGEINFO_H

JONJ_FUNCTION(getmyuid);
JONJ_FUNCTION(getmygid);
JONJ_FUNCTION(getmypid);
JONJ_FUNCTION(getmyinode);
JONJ_FUNCTION(getlastmod);

JONJAPI void jonj_statpage(TSRMLS_D);
JONJAPI long jonj_getlastmod(TSRMLS_D);
extern long jonj_getuid(TSRMLS_D);
extern long jonj_getgid(TSRMLS_D);

#endif
