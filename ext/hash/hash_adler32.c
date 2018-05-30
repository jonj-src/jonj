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
#include "jonj_hash_adler32.h"

JONJ_HASH_API void JONJ_ADLER32Init(JONJ_ADLER32_CTX *context)
{
	context->state = 1;
}

JONJ_HASH_API void JONJ_ADLER32Update(JONJ_ADLER32_CTX *context, const unsigned char *input, size_t len)
{
	jonj_hash_uint32 i, s[2];
	
	s[0] = context->state & 0xffff;
	s[1] = (context->state >> 16) & 0xffff;
	for (i = 0; i < len; ++i) {
		s[0] += input[i];
		s[1] += s[0];
		if (s[1]>=0x7fffffff)
		{
			s[0] = s[0] % 65521;
			s[1] = s[1] % 65521;
		}
	}
	s[0] = s[0] % 65521;
	s[1] = s[1] % 65521;
	context->state = s[0] + (s[1] << 16);
}

JONJ_HASH_API void JONJ_ADLER32Final(unsigned char digest[4], JONJ_ADLER32_CTX *context)
{
	digest[0] = (unsigned char) ((context->state >> 24) & 0xff);
	digest[1] = (unsigned char) ((context->state >> 16) & 0xff);
	digest[2] = (unsigned char) ((context->state >> 8) & 0xff);
	digest[3] = (unsigned char) (context->state & 0xff);
	context->state = 0;
}

JONJ_HASH_API int JONJ_ADLER32Copy(const jonj_hash_ops *ops, JONJ_ADLER32_CTX *orig_context, JONJ_ADLER32_CTX *copy_context)
{
	copy_context->state = orig_context->state;
	return SUCCESS;
}

const jonj_hash_ops jonj_hash_adler32_ops = {
	(jonj_hash_init_func_t) JONJ_ADLER32Init,
	(jonj_hash_update_func_t) JONJ_ADLER32Update,
	(jonj_hash_final_func_t) JONJ_ADLER32Final,
	(jonj_hash_copy_func_t) JONJ_ADLER32Copy,
	4, /* what to say here? */
	4,
	sizeof(JONJ_ADLER32_CTX)
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
