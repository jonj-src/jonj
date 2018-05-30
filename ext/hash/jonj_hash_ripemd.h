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
   | Author: Sara Golemon <pollita@jonj.tk>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HASH_RIPEMD_H
#define JONJ_HASH_RIPEMD_H
#include "ext/standard/basic_functions.h"

/* RIPEMD context. */
typedef struct {
	jonj_hash_uint32 state[4];		/* state (ABCD) */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} JONJ_RIPEMD128_CTX;

typedef struct {
	jonj_hash_uint32 state[5];		/* state (ABCD) */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} JONJ_RIPEMD160_CTX;

typedef struct {
	jonj_hash_uint32 state[8];		/* state (ABCD) */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} JONJ_RIPEMD256_CTX;

typedef struct {
	jonj_hash_uint32 state[10];		/* state (ABCD) */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} JONJ_RIPEMD320_CTX;

JONJ_HASH_API void JONJ_RIPEMD128Init(JONJ_RIPEMD128_CTX *);
JONJ_HASH_API void JONJ_RIPEMD128Update(JONJ_RIPEMD128_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_RIPEMD128Final(unsigned char[16], JONJ_RIPEMD128_CTX *);

JONJ_HASH_API void JONJ_RIPEMD160Init(JONJ_RIPEMD160_CTX *);
JONJ_HASH_API void JONJ_RIPEMD160Update(JONJ_RIPEMD160_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_RIPEMD160Final(unsigned char[20], JONJ_RIPEMD160_CTX *);

JONJ_HASH_API void JONJ_RIPEMD256Init(JONJ_RIPEMD256_CTX *);
JONJ_HASH_API void JONJ_RIPEMD256Update(JONJ_RIPEMD256_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_RIPEMD256Final(unsigned char[32], JONJ_RIPEMD256_CTX *);

JONJ_HASH_API void JONJ_RIPEMD320Init(JONJ_RIPEMD320_CTX *);
JONJ_HASH_API void JONJ_RIPEMD320Update(JONJ_RIPEMD320_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_RIPEMD320Final(unsigned char[40], JONJ_RIPEMD320_CTX *);

#endif /* JONJ_HASH_RIPEMD_H */
