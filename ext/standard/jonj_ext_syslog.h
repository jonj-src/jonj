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
   | Author: Stig Sæther Bakken <ssb@jonj.tk>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_EXT_SYSLOG_H
#define JONJ_EXT_SYSLOG_H

#ifdef HAVE_SYSLOG_H

#include "jonj_syslog.h"

JONJ_MINIT_FUNCTION(syslog);
JONJ_RINIT_FUNCTION(syslog);
#ifdef JONJ_WIN32
JONJ_RSHUTDOWN_FUNCTION(syslog);
#endif
JONJ_MSHUTDOWN_FUNCTION(syslog);

JONJ_FUNCTION(openlog);
JONJ_FUNCTION(syslog);
JONJ_FUNCTION(closelog);

#endif

#endif /* JONJ_EXT_SYSLOG_H */
