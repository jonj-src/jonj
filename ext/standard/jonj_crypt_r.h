/* $Id$ */
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
   | Authors: Pierre Alain Joye  <pajoye@jonj.tk                          |
   +----------------------------------------------------------------------+
 */

#ifndef _CRYPT_WIHN32_H_
#define _CRYPT_WIHN32_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "crypt_freesec.h"

#ifndef __const
#ifdef __GNUC__
#define __CONST __const
#else
#define __CONST
#endif
#else
#define __CONST __const
#endif

void jonj_init_crypt_r();
void jonj_shutdown_crypt_r();

extern void _crypt_extended_init_r(void);

/*JONJAPI char* crypt(const char *key, const char *salt);*/
JONJAPI char *jonj_crypt_r (const char *__key, const char *__salt, struct jonj_crypt_extended_data * __data);

#define MD5_HASH_MAX_LEN 120

#include "crypt_blowfish.h"

extern char * jonj_md5_crypt_r(const char *pw, const char *salt, char *out);
extern char * jonj_sha512_crypt_r (const char *key, const char *salt, char *buffer, int buflen);
extern char * jonj_sha256_crypt_r (const char *key, const char *salt, char *buffer, int buflen);

#ifdef __cplusplus
}
#endif

#endif /* _CRYPT_WIHN32_H_ */
