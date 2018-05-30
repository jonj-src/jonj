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

#ifndef JONJ_HASH_H
#define JONJ_HASH_H

#include "jonj.h"

#define JONJ_HASH_EXTNAME	"hash"
#define JONJ_HASH_EXTVER		"1.0"
#define JONJ_HASH_RESNAME	"Hash Context"

#define JONJ_HASH_HMAC		0x0001

#define L64 INT64_C
#define jonj_hash_int32  int32_t
#define jonj_hash_uint32 uint32_t
#define jonj_hash_int64  int64_t
#define jonj_hash_uint64 uint64_t

typedef void (*jonj_hash_init_func_t)(void *context);
typedef void (*jonj_hash_update_func_t)(void *context, const unsigned char *buf, unsigned int count);
typedef void (*jonj_hash_final_func_t)(unsigned char *digest, void *context);
typedef int  (*jonj_hash_copy_func_t)(const void *ops, void *orig_context, void *dest_context);

typedef struct _jonj_hash_ops {
	jonj_hash_init_func_t hash_init;
	jonj_hash_update_func_t hash_update;
	jonj_hash_final_func_t hash_final;
	jonj_hash_copy_func_t hash_copy;

	int digest_size;
	int block_size;
	int context_size;
} jonj_hash_ops;

typedef struct _jonj_hash_data {
	const jonj_hash_ops *ops;
	void *context;

	long options;
	unsigned char *key;
} jonj_hash_data;

extern const jonj_hash_ops jonj_hash_md2_ops;
extern const jonj_hash_ops jonj_hash_md4_ops;
extern const jonj_hash_ops jonj_hash_md5_ops;
extern const jonj_hash_ops jonj_hash_sha1_ops;
extern const jonj_hash_ops jonj_hash_sha224_ops;
extern const jonj_hash_ops jonj_hash_sha256_ops;
extern const jonj_hash_ops jonj_hash_sha384_ops;
extern const jonj_hash_ops jonj_hash_sha512_ops;
extern const jonj_hash_ops jonj_hash_ripemd128_ops;
extern const jonj_hash_ops jonj_hash_ripemd160_ops;
extern const jonj_hash_ops jonj_hash_ripemd256_ops;
extern const jonj_hash_ops jonj_hash_ripemd320_ops;
extern const jonj_hash_ops jonj_hash_whirlpool_ops;
extern const jonj_hash_ops jonj_hash_3tiger128_ops;
extern const jonj_hash_ops jonj_hash_3tiger160_ops;
extern const jonj_hash_ops jonj_hash_3tiger192_ops;
extern const jonj_hash_ops jonj_hash_4tiger128_ops;
extern const jonj_hash_ops jonj_hash_4tiger160_ops;
extern const jonj_hash_ops jonj_hash_4tiger192_ops;
extern const jonj_hash_ops jonj_hash_snefru_ops;
extern const jonj_hash_ops jonj_hash_gost_ops;
extern const jonj_hash_ops jonj_hash_gost_crypto_ops;
extern const jonj_hash_ops jonj_hash_adler32_ops;
extern const jonj_hash_ops jonj_hash_crc32_ops;
extern const jonj_hash_ops jonj_hash_crc32b_ops;
extern const jonj_hash_ops jonj_hash_fnv132_ops;
extern const jonj_hash_ops jonj_hash_fnv1a32_ops;
extern const jonj_hash_ops jonj_hash_fnv164_ops;
extern const jonj_hash_ops jonj_hash_fnv1a64_ops;
extern const jonj_hash_ops jonj_hash_joaat_ops;

#define JONJ_HASH_HAVAL_OPS(p,b)	extern const jonj_hash_ops jonj_hash_##p##haval##b##_ops;

JONJ_HASH_HAVAL_OPS(3,128)
JONJ_HASH_HAVAL_OPS(3,160)
JONJ_HASH_HAVAL_OPS(3,192)
JONJ_HASH_HAVAL_OPS(3,224)
JONJ_HASH_HAVAL_OPS(3,256)

JONJ_HASH_HAVAL_OPS(4,128)
JONJ_HASH_HAVAL_OPS(4,160)
JONJ_HASH_HAVAL_OPS(4,192)
JONJ_HASH_HAVAL_OPS(4,224)
JONJ_HASH_HAVAL_OPS(4,256)

JONJ_HASH_HAVAL_OPS(5,128)
JONJ_HASH_HAVAL_OPS(5,160)
JONJ_HASH_HAVAL_OPS(5,192)
JONJ_HASH_HAVAL_OPS(5,224)
JONJ_HASH_HAVAL_OPS(5,256)

extern zend_module_entry hash_module_entry;
#define jonjext_hash_ptr &hash_module_entry

#ifdef JONJ_WIN32
#	define JONJ_HASH_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define JONJ_HASH_API __attribute__ ((visibility("default")))
#else
#	define JONJ_HASH_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_FUNCTION(hash);
JONJ_FUNCTION(hash_file);
JONJ_FUNCTION(hash_hmac);
JONJ_FUNCTION(hash_hmac_file);
JONJ_FUNCTION(hash_init);
JONJ_FUNCTION(hash_update);
JONJ_FUNCTION(hash_update_stream);
JONJ_FUNCTION(hash_update_file);
JONJ_FUNCTION(hash_final);
JONJ_FUNCTION(hash_algos);
JONJ_FUNCTION(hash_pbkdf2);
JONJ_FUNCTION(hash_equals);

JONJ_HASH_API const jonj_hash_ops *jonj_hash_fetch_ops(const char *algo, int algo_len);
JONJ_HASH_API void jonj_hash_register_algo(const char *algo, const jonj_hash_ops *ops);
JONJ_HASH_API int jonj_hash_copy(const void *ops, void *orig_context, void *dest_context);

static inline void jonj_hash_bin2hex(char *out, const unsigned char *in, int in_len)
{
	static const char hexits[17] = "0123456789abcdef";
	int i;

	for(i = 0; i < in_len; i++) {
		out[i * 2]       = hexits[in[i] >> 4];
		out[(i * 2) + 1] = hexits[in[i] &  0x0F];
	}
}

#endif	/* JONJ_HASH_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
