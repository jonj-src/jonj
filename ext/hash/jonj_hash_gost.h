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
   | Author: Michael Wallner <mike@jonj.tk>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HASH_GOST_H
#define JONJ_HASH_GOST_H

#include "ext/standard/basic_functions.h"

/* GOST context */
typedef struct {
	jonj_hash_uint32 state[16];
	jonj_hash_uint32 count[2];
	unsigned char length;
	unsigned char buffer[32];
	const jonj_hash_uint32 (*tables)[4][256];
} JONJ_GOST_CTX;

JONJ_HASH_API void JONJ_GOSTInit(JONJ_GOST_CTX *);
JONJ_HASH_API void JONJ_GOSTUpdate(JONJ_GOST_CTX *, const unsigned char *, size_t);
JONJ_HASH_API void JONJ_GOSTFinal(unsigned char[64], JONJ_GOST_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
