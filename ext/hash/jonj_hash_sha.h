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
   | SHA1 Author: Stefan Esser <sesser@jonj.tk>                           |
   | SHA256 Author: Sara Golemon <pollita@jonj.tk>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HASH_SHA_H
#define JONJ_HASH_SHA_H

/* When SHA is removed from Core,
	the ext/standard/sha1.c file can be removed
	and the ext/standard/sha1.h file can be reduced to:
		#define JONJ_HASH_SHA1_NOT_IN_CORE
		#include "ext/hash/jonj_hash_sha.h"
	Don't forget to remove sha1() and sha1_file() from basic_functions.c
 */
#include "ext/standard/sha1.h"
#include "ext/standard/basic_functions.h"

#ifdef JONJ_HASH_SHA1_NOT_IN_CORE

/* SHA1 context. */
typedef struct {
	jonj_hash_uint32 state[5];		/* state (ABCD) */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} JONJ_SHA1_CTX;

JONJ_HASH_API void JONJ_SHA1Init(JONJ_SHA1_CTX *);
JONJ_HASH_API void JONJ_SHA1Update(JONJ_SHA1_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_SHA1Final(unsigned char[20], JONJ_SHA1_CTX *);

JONJ_FUNCTION(sha1);
JONJ_FUNCTION(sha1_file);

#endif /* JONJ_HASH_SHA1_NOT_IN_CORE */

/* SHA224 context. */
typedef struct {
	jonj_hash_uint32 state[8];		/* state */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} JONJ_SHA224_CTX;

JONJ_HASH_API void JONJ_SHA224Init(JONJ_SHA224_CTX *);
JONJ_HASH_API void JONJ_SHA224Update(JONJ_SHA224_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_SHA224Final(unsigned char[28], JONJ_SHA224_CTX *);

/* SHA256 context. */
typedef struct {
	jonj_hash_uint32 state[8];		/* state */
	jonj_hash_uint32 count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} JONJ_SHA256_CTX;

JONJ_HASH_API void JONJ_SHA256Init(JONJ_SHA256_CTX *);
JONJ_HASH_API void JONJ_SHA256Update(JONJ_SHA256_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_SHA256Final(unsigned char[32], JONJ_SHA256_CTX *);

/* SHA384 context */
typedef struct {
	jonj_hash_uint64 state[8];	/* state */
	jonj_hash_uint64 count[2];	/* number of bits, modulo 2^128 */
	unsigned char buffer[128];	/* input buffer */
} JONJ_SHA384_CTX;

JONJ_HASH_API void JONJ_SHA384Init(JONJ_SHA384_CTX *);
JONJ_HASH_API void JONJ_SHA384Update(JONJ_SHA384_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_SHA384Final(unsigned char[48], JONJ_SHA384_CTX *);

/* SHA512 context */
typedef struct {
	jonj_hash_uint64 state[8];	/* state */
	jonj_hash_uint64 count[2];	/* number of bits, modulo 2^128 */
	unsigned char buffer[128];	/* input buffer */
} JONJ_SHA512_CTX;

JONJ_HASH_API void JONJ_SHA512Init(JONJ_SHA512_CTX *);
JONJ_HASH_API void JONJ_SHA512Update(JONJ_SHA512_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_SHA512Final(unsigned char[64], JONJ_SHA512_CTX *);

#endif /* JONJ_HASH_SHA_H */
