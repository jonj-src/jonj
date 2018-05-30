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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   *         Pierre Joye <pierre@jonj.tk>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "jonj.h"
#include <wincrypt.h>

JONJAPI char *jonj_win32_error_to_msg(int error)
{
	char *buf = NULL;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |	FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	(LPTSTR)&buf, 0, NULL
	);

	return (buf ? (char *) buf : "");
}

int jonj_win32_check_trailing_space(const char * path, const int path_len) {
	if (path_len < 1) {
		return 1;
	}
	if (path) {
		if (path[0] == ' ' || path[path_len - 1] == ' ') {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 0;
	}
}

HCRYPTPROV   hCryptProv;
unsigned int has_crypto_ctx = 0;

#ifdef ZTS
MUTEX_T jonj_lock_win32_cryptoctx;
void jonj_win32_init_rng_lock()
{
	jonj_lock_win32_cryptoctx = tsrm_mutex_alloc();
}

void jonj_win32_free_rng_lock()
{
	tsrm_mutex_lock(jonj_lock_win32_cryptoctx);
	if (has_crypto_ctx == 1) {
		CryptReleaseContext(hCryptProv, 0);
		has_crypto_ctx = 0;
	}
	tsrm_mutex_unlock(jonj_lock_win32_cryptoctx);
	tsrm_mutex_free(jonj_lock_win32_cryptoctx);

}
#else
#define jonj_win32_init_rng_lock();
#define jonj_win32_free_rng_lock();
#endif



JONJAPI int jonj_win32_get_random_bytes(unsigned char *buf, size_t size) {  /* {{{ */

	unsigned int has_contextg = 0;

	BOOL ret;
	size_t i = 0;

#ifdef ZTS
	tsrm_mutex_lock(jonj_lock_win32_cryptoctx);
#endif

	if (has_crypto_ctx == 0) {
		/* CRYPT_VERIFYCONTEXT > only hashing&co-like use, no need to acces prv keys */
		if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET|CRYPT_VERIFYCONTEXT )) {
			/* Could mean that the key container does not exist, let try 
			   again by asking for a new one. If it fails here, it surely means that the user running 
               this process does not have the permission(s) to use this container.
             */
			if (GetLastError() == NTE_BAD_KEYSET) {
				if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET | CRYPT_VERIFYCONTEXT )) {
					has_crypto_ctx = 1;
				} else {
					has_crypto_ctx = 0;
				}
			}
		} else {
			has_crypto_ctx = 1;
		}
	}

#ifdef ZTS
	tsrm_mutex_unlock(jonj_lock_win32_cryptoctx);
#endif

	if (has_crypto_ctx == 0) {
		return FAILURE;
	}

	ret = CryptGenRandom(hCryptProv, size, buf);

	if (ret) {
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

