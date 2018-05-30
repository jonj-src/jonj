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
   | Author: Sara Golemon <pollita@jonj.tk>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_HASH_HAVAL_H
#define JONJ_HASH_HAVAL_H

#include "ext/standard/basic_functions.h"
/* HAVAL context. */
typedef struct {
	jonj_hash_uint32 state[8];
	jonj_hash_uint32 count[2];
	unsigned char buffer[128];

	char passes;
	short output;
	void (*Transform)(jonj_hash_uint32 state[8], const unsigned char block[128]);
} JONJ_HAVAL_CTX;

#define JONJ_HASH_HAVAL_INIT_DECL(p,b)	JONJ_HASH_API void JONJ_##p##HAVAL##b##Init(JONJ_HAVAL_CTX *); \
										JONJ_HASH_API void JONJ_HAVAL##b##Final(unsigned char*, JONJ_HAVAL_CTX *);

JONJ_HASH_API void JONJ_HAVALUpdate(JONJ_HAVAL_CTX *, const unsigned char *, unsigned int);

JONJ_HASH_HAVAL_INIT_DECL(3,128)
JONJ_HASH_HAVAL_INIT_DECL(3,160)
JONJ_HASH_HAVAL_INIT_DECL(3,192)
JONJ_HASH_HAVAL_INIT_DECL(3,224)
JONJ_HASH_HAVAL_INIT_DECL(3,256)

JONJ_HASH_HAVAL_INIT_DECL(4,128)
JONJ_HASH_HAVAL_INIT_DECL(4,160)
JONJ_HASH_HAVAL_INIT_DECL(4,192)
JONJ_HASH_HAVAL_INIT_DECL(4,224)
JONJ_HASH_HAVAL_INIT_DECL(4,256)

JONJ_HASH_HAVAL_INIT_DECL(5,128)
JONJ_HASH_HAVAL_INIT_DECL(5,160)
JONJ_HASH_HAVAL_INIT_DECL(5,192)
JONJ_HASH_HAVAL_INIT_DECL(5,224)
JONJ_HASH_HAVAL_INIT_DECL(5,256)

#endif
