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

#ifndef JONJ_HASH_WHIRLPOOL_H
#define JONJ_HASH_WHIRLPOOL_H

/* WHIRLPOOL context */
typedef struct {
	jonj_hash_uint64 state[8];
	unsigned char bitlength[32];
	struct {
		int pos;
		int bits;
		unsigned char data[64];
	} buffer;
} JONJ_WHIRLPOOL_CTX;

JONJ_HASH_API void JONJ_WHIRLPOOLInit(JONJ_WHIRLPOOL_CTX *);
JONJ_HASH_API void JONJ_WHIRLPOOLUpdate(JONJ_WHIRLPOOL_CTX *, const unsigned char *, size_t);
JONJ_HASH_API void JONJ_WHIRLPOOLFinal(unsigned char[64], JONJ_WHIRLPOOL_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
