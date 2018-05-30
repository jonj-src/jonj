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
  | Authors: Michael Wallner <mike@jonj.tk>                              |
  |          Sara Golemon <pollita@jonj.tk>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj_hash.h"
#include "jonj_hash_crc32.h"
#include "jonj_hash_crc32_tables.h"

JONJ_HASH_API void JONJ_CRC32Init(JONJ_CRC32_CTX *context)
{
	context->state = ~0;
}

JONJ_HASH_API void JONJ_CRC32Update(JONJ_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i;
	
	for (i = 0; i < len; ++i) {
		context->state = (context->state << 8) ^ crc32_table[(context->state >> 24) ^ (input[i] & 0xff)];
	}
}

JONJ_HASH_API void JONJ_CRC32BUpdate(JONJ_CRC32_CTX *context, const unsigned char *input, size_t len)
{
	size_t i;
	
	for (i = 0; i < len; ++i) {
		context->state = (context->state >> 8) ^ crc32b_table[(context->state ^ input[i]) & 0xff];
	}
}

JONJ_HASH_API void JONJ_CRC32Final(unsigned char digest[4], JONJ_CRC32_CTX *context)
{
	context->state=~context->state;
	digest[3] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[0] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

JONJ_HASH_API void JONJ_CRC32BFinal(unsigned char digest[4], JONJ_CRC32_CTX *context)
{
	context->state=~context->state;
	digest[0] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[3] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

JONJ_HASH_API int JONJ_CRC32Copy(const jonj_hash_ops *ops, JONJ_CRC32_CTX *orig_context, JONJ_CRC32_CTX *copy_context)
{
	copy_context->state = orig_context->state;
	return SUCCESS;
}

const jonj_hash_ops jonj_hash_crc32_ops = {
	(jonj_hash_init_func_t) JONJ_CRC32Init,
	(jonj_hash_update_func_t) JONJ_CRC32Update,
	(jonj_hash_final_func_t) JONJ_CRC32Final,
	(jonj_hash_copy_func_t) JONJ_CRC32Copy,
	4, /* what to say here? */
	4,
	sizeof(JONJ_CRC32_CTX)
};

const jonj_hash_ops jonj_hash_crc32b_ops = {
	(jonj_hash_init_func_t) JONJ_CRC32Init,
	(jonj_hash_update_func_t) JONJ_CRC32BUpdate,
	(jonj_hash_final_func_t) JONJ_CRC32BFinal,
	(jonj_hash_copy_func_t) JONJ_CRC32Copy,
	4, /* what to say here? */
	4,
	sizeof(JONJ_CRC32_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
