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
  | Author: Michael Maclean <mgdm@jonj.tk>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HASH_FNV_H
#define JONJ_HASH_FNV_H

#define JONJ_FNV1_32_INIT ((jonj_hash_uint32)0x811c9dc5)
#define JONJ_FNV1_32A_INIT JONJ_FNV1_32_INIT

#define JONJ_FNV_32_PRIME ((jonj_hash_uint32)0x01000193)

#define JONJ_FNV1_64_INIT ((jonj_hash_uint64)0xcbf29ce484222325ULL)
#define JONJ_FNV1A_64_INIT FNV1_64_INIT

#define JONJ_FNV_64_PRIME ((jonj_hash_uint64)0x100000001b3ULL)


/*
 * hash types
 */
enum jonj_fnv_type {
	JONJ_FNV_NONE  = 0,	/* invalid FNV hash type */
	JONJ_FNV0_32   = 1,	/* FNV-0 32 bit hash */
	JONJ_FNV1_32   = 2,	/* FNV-1 32 bit hash */
	JONJ_FNV1a_32  = 3,	/* FNV-1a 32 bit hash */
	JONJ_FNV0_64   = 4,	/* FNV-0 64 bit hash */
	JONJ_FNV1_64   = 5,	/* FNV-1 64 bit hash */
	JONJ_FNV1a_64  = 6,	/* FNV-1a 64 bit hash */
};

typedef struct {
	jonj_hash_uint32 state;
} JONJ_FNV132_CTX;

typedef struct {
	jonj_hash_uint64 state;
} JONJ_FNV164_CTX;


JONJ_HASH_API void JONJ_FNV132Init(JONJ_FNV132_CTX *context);
JONJ_HASH_API void JONJ_FNV132Update(JONJ_FNV132_CTX *context, const unsigned char *input, unsigned int inputLen);
JONJ_HASH_API void JONJ_FNV1a32Update(JONJ_FNV132_CTX *context, const unsigned char *input, unsigned int inputLen);
JONJ_HASH_API void JONJ_FNV132Final(unsigned char digest[16], JONJ_FNV132_CTX * context);

JONJ_HASH_API void JONJ_FNV164Init(JONJ_FNV164_CTX *context);
JONJ_HASH_API void JONJ_FNV164Update(JONJ_FNV164_CTX *context, const unsigned char *input, unsigned int inputLen);
JONJ_HASH_API void JONJ_FNV1a64Update(JONJ_FNV164_CTX *context, const unsigned char *input, unsigned int inputLen);
JONJ_HASH_API void JONJ_FNV164Final(unsigned char digest[16], JONJ_FNV164_CTX * context);

static jonj_hash_uint32 fnv_32_buf(void *buf, size_t len, jonj_hash_uint32 hval, int alternate);
static jonj_hash_uint64 fnv_64_buf(void *buf, size_t len, jonj_hash_uint64 hval, int alternate);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
