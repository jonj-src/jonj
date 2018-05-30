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

#ifndef JONJ_HASH_TIGER_H
#define JONJ_HASH_TIGER_H

/* TIGER context */
typedef struct {
	jonj_hash_uint64 state[3];
	jonj_hash_uint64 passed;
	unsigned char buffer[64];
	unsigned int passes:1;
	unsigned int length:7;
} JONJ_TIGER_CTX;

JONJ_HASH_API void JONJ_3TIGERInit(JONJ_TIGER_CTX *context);
JONJ_HASH_API void JONJ_4TIGERInit(JONJ_TIGER_CTX *context);
JONJ_HASH_API void JONJ_TIGERUpdate(JONJ_TIGER_CTX *context, const unsigned char *input, size_t len);
JONJ_HASH_API void JONJ_TIGER128Final(unsigned char digest[16], JONJ_TIGER_CTX *context);
JONJ_HASH_API void JONJ_TIGER160Final(unsigned char digest[20], JONJ_TIGER_CTX *context);
JONJ_HASH_API void JONJ_TIGER192Final(unsigned char digest[24], JONJ_TIGER_CTX *context);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
