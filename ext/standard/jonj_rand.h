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
   | Authors: Rasmus Lerdorf <rasmus@jonj.tk>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@jonj.tk>                          |
   |                                                                      |
   | Based on code from: Shawn Cokus <Cokus@math.washington.edu>          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef JONJ_RAND_H
#define	JONJ_RAND_H

#include <stdlib.h>
#include "basic_functions.h"
#include "jonj_lcg.h"

/* System Rand functions */
#ifndef RAND_MAX
#define RAND_MAX (1<<15)
#endif

/* In ZTS mode we rely on rand_r() so we must use RAND_MAX. */
#if !defined(ZTS) && (defined(HAVE_LRAND48) || defined(HAVE_RANDOM))
#define JONJ_RAND_MAX 2147483647
#else
#define JONJ_RAND_MAX RAND_MAX
#endif

#define RAND_RANGE(__n, __min, __max, __tmax) \
    (__n) = (__min) + (long) ((double) ( (double) (__max) - (__min) + 1.0) * ((__n) / ((__tmax) + 1.0)))

/* MT Rand */
#define JONJ_MT_RAND_MAX ((long) (0x7FFFFFFF)) /* (1<<31) - 1 */ 

#ifdef JONJ_WIN32
#define GENERATE_SEED() (((long) (time(0) * GetCurrentProcessId())) ^ ((long) (1000000.0 * jonj_combined_lcg(TSRMLS_C))))
#else
#define GENERATE_SEED() (((long) (time(0) * getpid())) ^ ((long) (1000000.0 * jonj_combined_lcg(TSRMLS_C))))
#endif

JONJAPI void jonj_srand(long seed TSRMLS_DC);
JONJAPI long jonj_rand(TSRMLS_D);
JONJAPI void jonj_mt_srand(jonj_uint32 seed TSRMLS_DC);
JONJAPI jonj_uint32 jonj_mt_rand(TSRMLS_D);

#endif	/* JONJ_RAND_H */
