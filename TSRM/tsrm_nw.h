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
   | Authors: Venkat Raghavan S <rvenkat@novell.com>                      |
   |          Anantha Kesari H Y <hyanantha@novell.com>                   |
   +----------------------------------------------------------------------+
*/


#ifndef TSRM_NW_H
#define TSRM_NW_H

#include "TSRM.h"

TSRM_API FILE* popen(const char *command, const char *type);
TSRM_API int pclose(FILE* stream);

#endif
