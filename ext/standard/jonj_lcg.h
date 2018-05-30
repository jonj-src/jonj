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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_LCG_H
#define JONJ_LCG_H

#include "ext/standard/basic_functions.h"

typedef struct {
	jonj_int32 s1;
	jonj_int32 s2;
	int seeded;
} jonj_lcg_globals;

JONJAPI double jonj_combined_lcg(TSRMLS_D);
JONJ_FUNCTION(lcg_value);

JONJ_MINIT_FUNCTION(lcg);

#ifdef ZTS
#define LCG(v) TSRMG(lcg_globals_id, jonj_lcg_globals *, v)
#else
#define LCG(v) (lcg_globals.v)
#endif

#endif
