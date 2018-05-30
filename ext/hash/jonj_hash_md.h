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
   | Original Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>               |
   | Modified for pHASH by: Sara Golemon <pollita@jonj.tk>
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HASH_MD_H
#define JONJ_HASH_MD_H

/* When SHA is removed from Core,
    the ext/standard/sha1.c file can be removed
    and the ext/standard/sha1.h file can be reduced to:
        #define JONJ_HASH_SHA1_NOT_IN_CORE
        #include "ext/hash/jonj_hash_sha.h"
	Don't forget to remove md5() and md5_file() entries from basic_functions.c
 */

#include "ext/standard/md5.h"

#ifdef JONJ_HASH_MD5_NOT_IN_CORE
/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

/* MD5 context. */
typedef struct {
	jonj_hash_uint32 state[4];				/* state (ABCD) */
	jonj_hash_uint32 count[2];				/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} JONJ_MD5_CTX;

JONJ_HASH_API void make_digest(char *md5str, unsigned char *digest);
JONJ_HASH_API void JONJ_MD5Init(JONJ_MD5_CTX *);
JONJ_HASH_API void JONJ_MD5Update(JONJ_MD5_CTX *, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_MD5Final(unsigned char[16], JONJ_MD5_CTX *);

JONJ_NAMED_FUNCTION(jonj_if_md5);
JONJ_NAMED_FUNCTION(jonj_if_md5_file);
#endif /* JONJ_HASH_MD5_NOT_IN_CORE */

/* MD4 context */
typedef struct {
	jonj_hash_uint32 state[4];
	jonj_hash_uint32 count[2];
	unsigned char buffer[64];
} JONJ_MD4_CTX;

JONJ_HASH_API void JONJ_MD4Init(JONJ_MD4_CTX *);
JONJ_HASH_API void JONJ_MD4Update(JONJ_MD4_CTX *context, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_MD4Final(unsigned char[16], JONJ_MD4_CTX *);

/* MD2 context */
typedef struct {
	unsigned char state[48];
	unsigned char checksum[16];
	unsigned char buffer[16];
	char in_buffer;
} JONJ_MD2_CTX;

JONJ_HASH_API void JONJ_MD2Init(JONJ_MD2_CTX *context);
JONJ_HASH_API void JONJ_MD2Update(JONJ_MD2_CTX *context, const unsigned char *, unsigned int);
JONJ_HASH_API void JONJ_MD2Final(unsigned char[16], JONJ_MD2_CTX *);

#endif
