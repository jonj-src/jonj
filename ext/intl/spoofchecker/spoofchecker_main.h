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
   | Authors: Scott MacVicar <scottmac@jonj.tk>                           |
   +----------------------------------------------------------------------+
 */

#ifndef SPOOFCHECKER_MAIN_H
#define SPOOFCHECKER_MAIN_H

#include <jonj.h>

JONJ_METHOD(Spoofchecker, isSuspicious);
JONJ_METHOD(Spoofchecker, areConfusable);
JONJ_METHOD(Spoofchecker, setAllowedLocales);
JONJ_METHOD(Spoofchecker, setChecks);

#endif // SPOOFCHECKER_MAIN_H
