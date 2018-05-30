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
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <signal.h>
#ifndef JONJ_SIGNAL_H
#define JONJ_SIGNAL_H

#ifndef NSIG
# define NSIG 32
#endif
#ifndef SIGRTMAX
# define SIGRTMAX 64
#endif

typedef void Sigfunc(int);
Sigfunc *jonj_signal(int signo, Sigfunc *func, int restart);
Sigfunc *jonj_signal4(int signo, Sigfunc *func, int restart, int mask_all);

#endif