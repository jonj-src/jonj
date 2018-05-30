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

#ifndef JONJ_HASH_ADLER32_H
#define JONJ_HASH_ADLER32_H

#include "ext/standard/basic_functions.h"

typedef struct {
	jonj_hash_uint32 state;
} JONJ_ADLER32_CTX;

JONJ_HASH_API void JONJ_ADLER32Init(JONJ_ADLER32_CTX *context);
JONJ_HASH_API void JONJ_ADLER32Update(JONJ_ADLER32_CTX *context, const unsigned char *input, size_t len);
JONJ_HASH_API void JONJ_ADLER32Final(unsigned char digest[4], JONJ_ADLER32_CTX *context);
JONJ_HASH_API int JONJ_ADLER32Copy(const jonj_hash_ops *ops, JONJ_ADLER32_CTX *orig_context, JONJ_ADLER32_CTX *copy_context);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
