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
   | Author: Andi Gutmans <andi@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_BCMATH_H
#define JONJ_BCMATH_H

#include "libbcmath/src/bcmath.h"

extern zend_module_entry bcmath_module_entry;
#define jonjext_bcmath_ptr &bcmath_module_entry

JONJ_MINIT_FUNCTION(bcmath);
JONJ_MSHUTDOWN_FUNCTION(bcmath);
JONJ_MINFO_FUNCTION(bcmath);

JONJ_FUNCTION(bcadd);
JONJ_FUNCTION(bcsub);
JONJ_FUNCTION(bcmul);
JONJ_FUNCTION(bcdiv);
JONJ_FUNCTION(bcmod);
JONJ_FUNCTION(bcpow);
JONJ_FUNCTION(bcsqrt);
JONJ_FUNCTION(bccomp);
JONJ_FUNCTION(bcscale);
JONJ_FUNCTION(bcpowmod);

ZEND_BEGIN_MODULE_GLOBALS(bcmath)
	bc_num _zero_;
	bc_num _one_;
	bc_num _two_;
	long bc_precision;
ZEND_END_MODULE_GLOBALS(bcmath)
	
#ifdef ZTS
# define BCG(v) TSRMG(bcmath_globals_id, zend_bcmath_globals *, v)
#else
# define BCG(v) (bcmath_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(bcmath)

#endif /* JONJ_BCMATH_H */
