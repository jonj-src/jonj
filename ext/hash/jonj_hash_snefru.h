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

#ifndef JONJ_HASH_SNEFRU_H
#define JONJ_HASH_SNEFRU_H

/* SNEFRU-2.5a with 8 passes and 256 bit hash output
 * AKA "Xerox Secure Hash Function" 
 */

#include "ext/standard/basic_functions.h"

/* SNEFRU context */
typedef struct {
	jonj_hash_uint32 state[16];
	jonj_hash_uint32 count[2];
	unsigned char length;
	unsigned char buffer[32];
} JONJ_SNEFRU_CTX;

JONJ_HASH_API void JONJ_SNEFRUInit(JONJ_SNEFRU_CTX *);
JONJ_HASH_API void JONJ_SNEFRUUpdate(JONJ_SNEFRU_CTX *, const unsigned char *, size_t);
JONJ_HASH_API void JONJ_SNEFRUFinal(unsigned char[32], JONJ_SNEFRU_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
