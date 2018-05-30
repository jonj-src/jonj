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
   | Author: Stefan Esser <sesser@jonj.tk>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef SHA1_H
#define SHA1_H

#include "ext/standard/basic_functions.h"

/* SHA1 context. */
typedef struct {
	jonj_uint32 state[5];		/* state (ABCD) */
	jonj_uint32 count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} JONJ_SHA1_CTX;

JONJAPI void JONJ_SHA1Init(JONJ_SHA1_CTX *);
JONJAPI void JONJ_SHA1Update(JONJ_SHA1_CTX *, const unsigned char *, unsigned int);
JONJAPI void JONJ_SHA1Final(unsigned char[20], JONJ_SHA1_CTX *);
JONJAPI void make_sha1_digest(char *sha1str, unsigned char *digest);

JONJ_FUNCTION(sha1);
JONJ_FUNCTION(sha1_file);

#endif
