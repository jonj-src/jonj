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

/* $Id*/

#ifndef JONJ_HASH_JOAAT_H
#define JONJ_HASH_JOAAT_H

typedef struct {
	jonj_hash_uint32 state;
} JONJ_JOAAT_CTX;

JONJ_HASH_API void JONJ_JOAATInit(JONJ_JOAAT_CTX *context);
JONJ_HASH_API void JONJ_JOAATUpdate(JONJ_JOAAT_CTX *context, const unsigned char *input, unsigned int inputLen);
JONJ_HASH_API void JONJ_JOAATFinal(unsigned char digest[16], JONJ_JOAAT_CTX * context);

static jonj_hash_uint32 joaat_buf(void *buf, size_t len, jonj_hash_uint32 hval);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
