/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2009 The JONJ Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Pierre A. Joye <pierre@jonj.tk>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ s*/

#ifndef IDN_IDN_H
#define IDN_IDN_H

#include <jonj.h>

JONJ_FUNCTION(idn_to_ascii);
JONJ_FUNCTION(idn_to_utf8);

void idn_register_constants(INIT_FUNC_ARGS);

#endif /* IDN_IDN_H */
