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

/*  Based on the public domain algorithm found at
	http://www.isthe.com/chongo/tech/comp/fnv/index.html */

#include "jonj_hash.h"
#include "jonj_hash_fnv.h"

const jonj_hash_ops jonj_hash_fnv132_ops = {
	(jonj_hash_init_func_t) JONJ_FNV132Init,
	(jonj_hash_update_func_t) JONJ_FNV132Update,
	(jonj_hash_final_func_t) JONJ_FNV132Final,
	(jonj_hash_copy_func_t) jonj_hash_copy,
	4,
	4,
	sizeof(JONJ_FNV132_CTX)
};

	const jonj_hash_ops jonj_hash_fnv1a32_ops = {
	(jonj_hash_init_func_t) JONJ_FNV132Init,
	(jonj_hash_update_func_t) JONJ_FNV1a32Update,
 	(jonj_hash_final_func_t) JONJ_FNV132Final, 
	(jonj_hash_copy_func_t) jonj_hash_copy,
	4,
	4,
	sizeof(JONJ_FNV132_CTX)
};

const jonj_hash_ops jonj_hash_fnv164_ops = {
	(jonj_hash_init_func_t) JONJ_FNV164Init,
	(jonj_hash_update_func_t) JONJ_FNV164Update,
	(jonj_hash_final_func_t) JONJ_FNV164Final, 
	(jonj_hash_copy_func_t) jonj_hash_copy,
	8,
	4,
	sizeof(JONJ_FNV164_CTX)
};

const jonj_hash_ops jonj_hash_fnv1a64_ops = {
	(jonj_hash_init_func_t) JONJ_FNV164Init,
	(jonj_hash_update_func_t) JONJ_FNV1a64Update,
 	(jonj_hash_final_func_t) JONJ_FNV164Final,  
	(jonj_hash_copy_func_t) jonj_hash_copy,
	8,
	4,
	sizeof(JONJ_FNV164_CTX)
};

/* {{{ JONJ_FNV132Init
 * 32-bit FNV-1 hash initialisation
 */
JONJ_HASH_API void JONJ_FNV132Init(JONJ_FNV132_CTX *context)
{
	context->state = JONJ_FNV1_32_INIT;
}
/* }}} */

JONJ_HASH_API void JONJ_FNV132Update(JONJ_FNV132_CTX *context, const unsigned char *input,
		unsigned int inputLen)
{
	context->state = fnv_32_buf((void *)input, inputLen, context->state, 0);
}

JONJ_HASH_API void JONJ_FNV1a32Update(JONJ_FNV132_CTX *context, const unsigned char *input,
		unsigned int inputLen)
{
	context->state = fnv_32_buf((void *)input, inputLen, context->state, 1);
}

JONJ_HASH_API void JONJ_FNV132Final(unsigned char digest[4], JONJ_FNV132_CTX * context)
{
#ifdef WORDS_BIGENDIAN
	memcpy(digest, &context->state, 4);
#else
	int i = 0;
	unsigned char *c = (unsigned char *) &context->state;

	for (i = 0; i < 4; i++) {
		digest[i] = c[3 - i];
	}
#endif
}

/* {{{ JONJ_FNV164Init
 * 64-bit FNV-1 hash initialisation
 */
JONJ_HASH_API void JONJ_FNV164Init(JONJ_FNV164_CTX *context)
{
	context->state = JONJ_FNV1_64_INIT;
}
/* }}} */

JONJ_HASH_API void JONJ_FNV164Update(JONJ_FNV164_CTX *context, const unsigned char *input,
		unsigned int inputLen)
{
	context->state = fnv_64_buf((void *)input, inputLen, context->state, 0);
}

JONJ_HASH_API void JONJ_FNV1a64Update(JONJ_FNV164_CTX *context, const unsigned char *input,
		unsigned int inputLen)
{
	context->state = fnv_64_buf((void *)input, inputLen, context->state, 1);
}

JONJ_HASH_API void JONJ_FNV164Final(unsigned char digest[8], JONJ_FNV164_CTX * context)
{
#ifdef WORDS_BIGENDIAN
	memcpy(digest, &context->state, 8);
#else
	int i = 0;
	unsigned char *c = (unsigned char *) &context->state;

	for (i = 0; i < 8; i++) {
		digest[i] = c[7 - i];
	}
#endif
}


/*
 * fnv_32_buf - perform a 32 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *  hval	- previous hash value or 0 if first call
 *  alternate - if > 0 use the alternate version
 *
 * returns:
 *  32 bit hash as a static hash type
 */
static jonj_hash_uint32
fnv_32_buf(void *buf, size_t len, jonj_hash_uint32 hval, int alternate)
{
	unsigned char *bp = (unsigned char *)buf;   /* start of buffer */
	unsigned char *be = bp + len;	   /* beyond end of buffer */

	/*
	 * FNV-1 hash each octet in the buffer
	 */
	while (bp < be) {

		if (alternate == 0) {
			/* multiply by the 32 bit FNV magic prime mod 2^32 */
			hval *= JONJ_FNV_32_PRIME;

			/* xor the bottom with the current octet */
			hval ^= (jonj_hash_uint32)*bp++;
		} else {
			/* xor the bottom with the current octet */
			hval ^= (jonj_hash_uint32)*bp++;

			/* multiply by the 32 bit FNV magic prime mod 2^32 */
			hval *= JONJ_FNV_32_PRIME;
		}
	}

	/* return our new hash value */
	return hval;
}

/*
 * fnv_64_buf - perform a 64 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *  hval	- previous hash value or 0 if first call
 *  alternate - if > 0 use the alternate version
 *
 * returns:
 *  64 bit hash as a static hash type
 */
static jonj_hash_uint64
fnv_64_buf(void *buf, size_t len, jonj_hash_uint64 hval, int alternate)
{
	unsigned char *bp = (unsigned char *)buf;   /* start of buffer */
	unsigned char *be = bp + len;	   /* beyond end of buffer */

	/*
	 * FNV-1 hash each octet of the buffer
	 */
	while (bp < be) {

		if (alternate == 0) {
			/* multiply by the 64 bit FNV magic prime mod 2^64 */
			hval *= JONJ_FNV_64_PRIME;

			/* xor the bottom with the current octet */
			hval ^= (jonj_hash_uint64)*bp++;
		 } else {
			/* xor the bottom with the current octet */
			hval ^= (jonj_hash_uint64)*bp++;

			/* multiply by the 64 bit FNV magic prime mod 2^64 */
			hval *= JONJ_FNV_64_PRIME;
		 }
	}

	/* return our new hash value */
	return hval;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
