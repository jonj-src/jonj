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
  | Author: Martin Jansen <mj@jonj.tk>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Implements Jenkins's one-at-a-time hashing algorithm as presented on
 * http://www.burtleburtle.net/bob/hash/doobs.html.
 */

#include "jonj_hash.h"
#include "jonj_hash_joaat.h"

const jonj_hash_ops jonj_hash_joaat_ops = {
	(jonj_hash_init_func_t) JONJ_JOAATInit,
	(jonj_hash_update_func_t) JONJ_JOAATUpdate,
	(jonj_hash_final_func_t) JONJ_JOAATFinal,
	(jonj_hash_copy_func_t) jonj_hash_copy,
	4,
	4,
	sizeof(JONJ_JOAAT_CTX)
};

JONJ_HASH_API void JONJ_JOAATInit(JONJ_JOAAT_CTX *context)
{
	context->state = 0;
}

JONJ_HASH_API void JONJ_JOAATUpdate(JONJ_JOAAT_CTX *context, const unsigned char *input, unsigned int inputLen)
{
	context->state = joaat_buf((void *)input, inputLen, context->state);
}

JONJ_HASH_API void JONJ_JOAATFinal(unsigned char digest[4], JONJ_JOAAT_CTX * context)
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
    context->state = 0;
}

/*
 * joaat_buf - perform a Jenkins's one-at-a-time hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *
 * returns:
 *  32 bit hash as a static hash type
 */
static jonj_hash_uint32
joaat_buf(void *buf, size_t len, jonj_hash_uint32 hval)
{
    size_t i;
    unsigned char *input = (unsigned char *)buf;
 
    for (i = 0; i < len; i++) {
        hval += input[i];
        hval += (hval << 10);
        hval ^= (hval >> 6);
    }

    hval += (hval << 3);
    hval ^= (hval >> 11);
    hval += (hval << 15);

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
