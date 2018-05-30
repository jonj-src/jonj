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
  |         Scott MacVicar <scottmac@jonj.tk>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include "jonj_hash.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"

static int jonj_hash_le_hash;
HashTable jonj_hash_hashtable;

#if (JONJ_MAJOR_VERSION >= 5)
# define DEFAULT_CONTEXT FG(default_context)
#else
# define DEFAULT_CONTEXT NULL
#endif

#ifdef JONJ_MHASH_BC
struct mhash_bc_entry {
	char *mhash_name;
	char *hash_name;
	int value;
};

#define MHASH_NUM_ALGOS 34

static struct mhash_bc_entry mhash_to_hash[MHASH_NUM_ALGOS] = {
	{"CRC32", "crc32", 0},
	{"MD5", "md5", 1},
	{"SHA1", "sha1", 2},
	{"HAVAL256", "haval256,3", 3},
	{NULL, NULL, 4},
	{"RIPEMD160", "ripemd160", 5},
	{NULL, NULL, 6},
	{"TIGER", "tiger192,3", 7},
	{"GOST", "gost", 8},
	{"CRC32B", "crc32b", 9},
	{"HAVAL224", "haval224,3", 10},
	{"HAVAL192", "haval192,3", 11},
	{"HAVAL160", "haval160,3", 12},
	{"HAVAL128", "haval128,3", 13},
	{"TIGER128", "tiger128,3", 14},
	{"TIGER160", "tiger160,3", 15},
	{"MD4", "md4", 16},
	{"SHA256", "sha256", 17},
	{"ADLER32", "adler32", 18},
	{"SHA224", "sha224", 19},
	{"SHA512", "sha512", 20},
	{"SHA384", "sha384", 21},
	{"WHIRLPOOL", "whirlpool", 22},
	{"RIPEMD128", "ripemd128", 23},
	{"RIPEMD256", "ripemd256", 24},
	{"RIPEMD320", "ripemd320", 25},
	{NULL, NULL, 26}, /* support needs to be added for snefru 128 */
	{"SNEFRU256", "snefru256", 27},
	{"MD2", "md2", 28},
	{"FNV132", "fnv132", 29},
	{"FNV1A32", "fnv1a32", 30},
	{"FNV164", "fnv164", 31},
	{"FNV1A64", "fnv1a64", 32},
	{"JOAAT", "joaat", 33},
};
#endif

/* Hash Registry Access */

JONJ_HASH_API const jonj_hash_ops *jonj_hash_fetch_ops(const char *algo, int algo_len) /* {{{ */
{
	jonj_hash_ops *ops;
	char *lower = estrndup(algo, algo_len);

	zend_str_tolower(lower, algo_len);
	if (SUCCESS != zend_hash_find(&jonj_hash_hashtable, lower, algo_len + 1, (void*)&ops)) {
		ops = NULL;
	}
	efree(lower);

	return ops;
}
/* }}} */

JONJ_HASH_API void jonj_hash_register_algo(const char *algo, const jonj_hash_ops *ops) /* {{{ */
{
	int algo_len = strlen(algo);
	char *lower = estrndup(algo, algo_len);
	
	zend_str_tolower(lower, algo_len);
	zend_hash_add(&jonj_hash_hashtable, lower, algo_len + 1, (void*)ops, sizeof(jonj_hash_ops), NULL);
	efree(lower);
}
/* }}} */

JONJ_HASH_API int jonj_hash_copy(const void *ops, void *orig_context, void *dest_context) /* {{{ */
{
	jonj_hash_ops *hash_ops = (jonj_hash_ops *)ops;

	memcpy(dest_context, orig_context, hash_ops->context_size);
	return SUCCESS;
}
/* }}} */

/* Userspace */

static void jonj_hash_do_hash(INTERNAL_FUNCTION_PARAMETERS, int isfilename, zend_bool raw_output_default) /* {{{ */
{
	char *algo, *data, *digest;
	int algo_len, data_len;
	zend_bool raw_output = raw_output_default;
	const jonj_hash_ops *ops;
	void *context;
	jonj_stream *stream = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &algo, &algo_len, &data, &data_len, &raw_output) == FAILURE) {
		return;
	}

	ops = jonj_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}
	if (isfilename) {
		if (CHECK_NULL_PATH(data, data_len)) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path");
			RETURN_FALSE;
		}
		stream = jonj_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS, NULL, DEFAULT_CONTEXT);
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	if (isfilename) {
		char buf[1024];
		int n;

		while ((n = jonj_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, (unsigned char *) buf, n);
		}
		jonj_stream_close(stream);
	} else {
		ops->hash_update(context, (unsigned char *) data, data_len);
	}

	digest = emalloc(ops->digest_size + 1);
	ops->hash_final((unsigned char *) digest, context);
	efree(context);

	if (raw_output) {
		digest[ops->digest_size] = 0;
		RETURN_STRINGL(digest, ops->digest_size, 0);
	} else {
		char *hex_digest = safe_emalloc(ops->digest_size, 2, 1);

		jonj_hash_bin2hex(hex_digest, (unsigned char *) digest, ops->digest_size);
		hex_digest[2 * ops->digest_size] = 0;
		efree(digest);
		RETURN_STRINGL(hex_digest, 2 * ops->digest_size, 0);
	}
}
/* }}} */

/* {{{ proto string hash(string algo, string data[, bool raw_output = false])
Generate a hash of a given input string
Returns lowercase hexits by default */
JONJ_FUNCTION(hash)
{
	jonj_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto string hash_file(string algo, string filename[, bool raw_output = false])
Generate a hash of a given file
Returns lowercase hexits by default */
JONJ_FUNCTION(hash_file)
{
	jonj_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

static inline void jonj_hash_string_xor_char(unsigned char *out, const unsigned char *in, const unsigned char xor_with, const int length) {
	int i;
	for (i=0; i < length; i++) {
		out[i] = in[i] ^ xor_with;
	}
}

static inline void jonj_hash_string_xor(unsigned char *out, const unsigned char *in, const unsigned char *xor_with, const int length) {
	int i;
	for (i=0; i < length; i++) {
		out[i] = in[i] ^ xor_with[i];
	}
}

static inline void jonj_hash_hmac_prep_key(unsigned char *K, const jonj_hash_ops *ops, void *context, const unsigned char *key, const int key_len) {
	memset(K, 0, ops->block_size);
	if (key_len > ops->block_size) {
		/* Reduce the key first */
		ops->hash_init(context);
		ops->hash_update(context, key, key_len);
		ops->hash_final(K, context);
	} else {
		memcpy(K, key, key_len);
	}
	/* XOR the key with 0x36 to get the ipad) */
	jonj_hash_string_xor_char(K, K, 0x36, ops->block_size);
}

static inline void jonj_hash_hmac_round(unsigned char *final, const jonj_hash_ops *ops, void *context, const unsigned char *key, const unsigned char *data, const long data_size) {
	ops->hash_init(context);
	ops->hash_update(context, key, ops->block_size);
	ops->hash_update(context, data, data_size);
	ops->hash_final(final, context);
}

static void jonj_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAMETERS, int isfilename, zend_bool raw_output_default) /* {{{ */
{
	char *algo, *data, *digest, *key, *K;
	int algo_len, data_len, key_len;
	zend_bool raw_output = raw_output_default;
	const jonj_hash_ops *ops;
	void *context;
	jonj_stream *stream = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|b", &algo, &algo_len, &data, &data_len, 
																  &key, &key_len, &raw_output) == FAILURE) {
		return;
	}

	ops = jonj_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}
	if (isfilename) {
		if (CHECK_NULL_PATH(data, data_len)) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path");
			RETURN_FALSE;
		}
		stream = jonj_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS, NULL, DEFAULT_CONTEXT);
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = emalloc(ops->context_size);

	K = emalloc(ops->block_size);
	digest = emalloc(ops->digest_size + 1);

	jonj_hash_hmac_prep_key((unsigned char *) K, ops, context, (unsigned char *) key, key_len);		

	if (isfilename) {
		char buf[1024];
		int n;
		ops->hash_init(context);
		ops->hash_update(context, (unsigned char *) K, ops->block_size);
		while ((n = jonj_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, (unsigned char *) buf, n);
		}
		jonj_stream_close(stream);
		ops->hash_final((unsigned char *) digest, context);
	} else {
		jonj_hash_hmac_round((unsigned char *) digest, ops, context, (unsigned char *) K, (unsigned char *) data, data_len);
	}

	jonj_hash_string_xor_char((unsigned char *) K, (unsigned char *) K, 0x6A, ops->block_size);

	jonj_hash_hmac_round((unsigned char *) digest, ops, context, (unsigned char *) K, (unsigned char *) digest, ops->digest_size);

	/* Zero the key */
	memset(K, 0, ops->block_size);
	efree(K);
	efree(context);

	if (raw_output) {
		digest[ops->digest_size] = 0;
		RETURN_STRINGL(digest, ops->digest_size, 0);
	} else {
		char *hex_digest = safe_emalloc(ops->digest_size, 2, 1);

		jonj_hash_bin2hex(hex_digest, (unsigned char *) digest, ops->digest_size);
		hex_digest[2 * ops->digest_size] = 0;
		efree(digest);
		RETURN_STRINGL(hex_digest, 2 * ops->digest_size, 0);
	}
}
/* }}} */

/* {{{ proto string hash_hmac(string algo, string data, string key[, bool raw_output = false])
Generate a hash of a given input string with a key using HMAC
Returns lowercase hexits by default */
JONJ_FUNCTION(hash_hmac)
{
	jonj_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto string hash_hmac_file(string algo, string filename, string key[, bool raw_output = false])
Generate a hash of a given file with a key using HMAC
Returns lowercase hexits by default */
JONJ_FUNCTION(hash_hmac_file)
{
	jonj_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */


/* {{{ proto resource hash_init(string algo[, int options, string key])
Initialize a hashing context */
JONJ_FUNCTION(hash_init)
{
	char *algo, *key = NULL;
	int algo_len, key_len = 0, argc = ZEND_NUM_ARGS();
	long options = 0;
	void *context;
	const jonj_hash_ops *ops;
	jonj_hash_data *hash;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|ls", &algo, &algo_len, &options, &key, &key_len) == FAILURE) {
		return;
	}

	ops = jonj_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}

	if (options & JONJ_HASH_HMAC &&
		key_len <= 0) {
		/* Note: a zero length key is no key at all */
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "HMAC requested without a key");
		RETURN_FALSE;
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	hash = emalloc(sizeof(jonj_hash_data));
	hash->ops = ops;
	hash->context = context;
	hash->options = options;
	hash->key = NULL;

	if (options & JONJ_HASH_HMAC) {
		char *K = emalloc(ops->block_size);
		int i;

		memset(K, 0, ops->block_size);

		if (key_len > ops->block_size) {
			/* Reduce the key first */
			ops->hash_update(context, (unsigned char *) key, key_len);
			ops->hash_final((unsigned char *) K, context);
			/* Make the context ready to start over */
			ops->hash_init(context);
		} else {
			memcpy(K, key, key_len);
		}
			
		/* XOR ipad */
		for(i=0; i < ops->block_size; i++) {
			K[i] ^= 0x36;
		}
		ops->hash_update(context, (unsigned char *) K, ops->block_size);
		hash->key = (unsigned char *) K;
	}

	ZEND_REGISTER_RESOURCE(return_value, hash, jonj_hash_le_hash);
}
/* }}} */

/* {{{ proto bool hash_update(resource context, string data)
Pump data into the hashing algorithm */
JONJ_FUNCTION(hash_update)
{
	zval *zhash;
	jonj_hash_data *hash;
	char *data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zhash, &data, &data_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, jonj_hash_data*, &zhash, -1, JONJ_HASH_RESNAME, jonj_hash_le_hash);

	hash->ops->hash_update(hash->context, (unsigned char *) data, data_len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int hash_update_stream(resource context, resource handle[, integer length])
Pump data into the hashing algorithm from an open stream */
JONJ_FUNCTION(hash_update_stream)
{
	zval *zhash, *zstream;
	jonj_hash_data *hash;
	jonj_stream *stream = NULL;
	long length = -1, didread = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|l", &zhash, &zstream, &length) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, jonj_hash_data*, &zhash, -1, JONJ_HASH_RESNAME, jonj_hash_le_hash);
	jonj_stream_from_zval(stream, &zstream);

	while (length) {
		char buf[1024];
		long n, toread = 1024;

		if (length > 0 && toread > length) {
			toread = length;
		}

		if ((n = jonj_stream_read(stream, buf, toread)) <= 0) {
			/* Nada mas */
			RETURN_LONG(didread);
		}
		hash->ops->hash_update(hash->context, (unsigned char *) buf, n);
		length -= n;
		didread += n;
	} 

	RETURN_LONG(didread);
}
/* }}} */

/* {{{ proto bool hash_update_file(resource context, string filename[, resource context])
Pump data into the hashing algorithm from a file */
JONJ_FUNCTION(hash_update_file)
{
	zval *zhash, *zcontext = NULL;
	jonj_hash_data *hash;
	jonj_stream_context *context;
	jonj_stream *stream;
	char *filename, buf[1024];
	int filename_len, n;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp|r", &zhash, &filename, &filename_len, &zcontext) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, jonj_hash_data*, &zhash, -1, JONJ_HASH_RESNAME, jonj_hash_le_hash);
	context = jonj_stream_context_from_zval(zcontext, 0);

	stream = jonj_stream_open_wrapper_ex(filename, "rb", REPORT_ERRORS, NULL, context);
	if (!stream) {
		/* Stream will report errors opening file */
		RETURN_FALSE;
	}

	while ((n = jonj_stream_read(stream, buf, sizeof(buf))) > 0) {
		hash->ops->hash_update(hash->context, (unsigned char *) buf, n);
	}
	jonj_stream_close(stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hash_final(resource context[, bool raw_output=false])
Output resulting digest */
JONJ_FUNCTION(hash_final)
{
	zval *zhash;
	jonj_hash_data *hash;
	zend_bool raw_output = 0;
	zend_rsrc_list_entry *le;
	char *digest;
	int digest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|b", &zhash, &raw_output) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, jonj_hash_data*, &zhash, -1, JONJ_HASH_RESNAME, jonj_hash_le_hash);

	digest_len = hash->ops->digest_size;
	digest = emalloc(digest_len + 1);
	hash->ops->hash_final((unsigned char *) digest, hash->context);
	if (hash->options & JONJ_HASH_HMAC) {
		int i;

		/* Convert K to opad -- 0x6A = 0x36 ^ 0x5C */
		for(i=0; i < hash->ops->block_size; i++) {
			hash->key[i] ^= 0x6A;
		}

		/* Feed this result into the outter hash */
		hash->ops->hash_init(hash->context);
		hash->ops->hash_update(hash->context, (unsigned char *) hash->key, hash->ops->block_size);
		hash->ops->hash_update(hash->context, (unsigned char *) digest, hash->ops->digest_size);
		hash->ops->hash_final((unsigned char *) digest, hash->context);

		/* Zero the key */
		memset(hash->key, 0, hash->ops->block_size);
		efree(hash->key);
		hash->key = NULL;
	}
	digest[digest_len] = 0;
	efree(hash->context);
	hash->context = NULL;

	/* zend_list_REAL_delete() */
	if (zend_hash_index_find(&EG(regular_list), Z_RESVAL_P(zhash), (void *) &le)==SUCCESS) {
		/* This is a hack to avoid letting the resource hide elsewhere (like in separated vars)
			FETCH_RESOURCE is intelligent enough to handle dealing with any issues this causes */
		le->refcount = 1;
	} /* FAILURE is not an option */
	zend_list_delete(Z_RESVAL_P(zhash));

	if (raw_output) {
		RETURN_STRINGL(digest, digest_len, 0);
	} else {
		char *hex_digest = safe_emalloc(digest_len,2,1);

		jonj_hash_bin2hex(hex_digest, (unsigned char *) digest, digest_len);
		hex_digest[2 * digest_len] = 0;
		efree(digest);
		RETURN_STRINGL(hex_digest, 2 * digest_len, 0);		
	}
}
/* }}} */

/* {{{ proto resource hash_copy(resource context)
Copy hash resource */
JONJ_FUNCTION(hash_copy)
{
	zval *zhash;
	jonj_hash_data *hash, *copy_hash;
	void *context;
	int res;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zhash) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, jonj_hash_data*, &zhash, -1, JONJ_HASH_RESNAME, jonj_hash_le_hash);


	context = emalloc(hash->ops->context_size);
	hash->ops->hash_init(context);

	res = hash->ops->hash_copy(hash->ops, hash->context, context);
	if (res != SUCCESS) {
		efree(context);
		RETURN_FALSE;
	}

	copy_hash = emalloc(sizeof(jonj_hash_data));
	copy_hash->ops = hash->ops;
	copy_hash->context = context;
	copy_hash->options = hash->options;
	copy_hash->key = ecalloc(1, hash->ops->block_size);
	if (hash->key) {
		memcpy(copy_hash->key, hash->key, hash->ops->block_size);
	}
	ZEND_REGISTER_RESOURCE(return_value, copy_hash, jonj_hash_le_hash);
}
/* }}} */

/* {{{ proto array hash_algos(void)
Return a list of registered hashing algorithms */
JONJ_FUNCTION(hash_algos)
{
	HashPosition pos;
	char *str;
	uint str_len;
	long type;
	ulong idx;

	array_init(return_value);
	for(zend_hash_internal_pointer_reset_ex(&jonj_hash_hashtable, &pos);
		(type = zend_hash_get_current_key_ex(&jonj_hash_hashtable, &str, &str_len, &idx, 0, &pos)) != HASH_KEY_NON_EXISTENT;
		zend_hash_move_forward_ex(&jonj_hash_hashtable, &pos)) {
		add_next_index_stringl(return_value, str, str_len-1, 1);
	}
}
/* }}} */

/* {{{ proto string hash_pbkdf2(string algo, string password, string salt, int iterations [, int length = 0, bool raw_output = false])
Generate a PBKDF2 hash of the given password and salt
Returns lowercase hexits by default */
JONJ_FUNCTION(hash_pbkdf2)
{
	char *returnval, *algo, *salt, *pass;
	unsigned char *computed_salt, *digest, *temp, *result, *K1, *K2;
	long loops, i, j, iterations, length = 0, digest_length;
	int algo_len, pass_len, salt_len;
	zend_bool raw_output = 0;
	const jonj_hash_ops *ops;
	void *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|lb", &algo, &algo_len, &pass, &pass_len, &salt, &salt_len, &iterations, &length, &raw_output) == FAILURE) {
		return;
	}

	ops = jonj_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}

	if (iterations <= 0) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Iterations must be a positive integer: %ld", iterations);
		RETURN_FALSE;
	}

	if (length < 0) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Length must be greater than or equal to 0: %ld", length);
		RETURN_FALSE;
	}

	if (salt_len > INT_MAX - 4) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Supplied salt is too long, max of INT_MAX - 4 bytes: %d supplied", salt_len);
		RETURN_FALSE;
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	K1 = emalloc(ops->block_size);
	K2 = emalloc(ops->block_size);
	digest = emalloc(ops->digest_size);
	temp = emalloc(ops->digest_size);

	/* Setup Keys that will be used for all hmac rounds */
	jonj_hash_hmac_prep_key(K1, ops, context, (unsigned char *) pass, pass_len);
	/* Convert K1 to opad -- 0x6A = 0x36 ^ 0x5C */
	jonj_hash_string_xor_char(K2, K1, 0x6A, ops->block_size);

	/* Setup Main Loop to build a long enough result */
	if (length == 0) {
		length = ops->digest_size;
		if (!raw_output) {
			length = length * 2;
		}
	}
	digest_length = length;
	if (!raw_output) {
		digest_length = (long) ceil((float) length / 2.0);
	}

	loops = (long) ceil((float) digest_length / (float) ops->digest_size);

	result = safe_emalloc(loops, ops->digest_size, 0);

	computed_salt = safe_emalloc(salt_len, 1, 4);
	memcpy(computed_salt, (unsigned char *) salt, salt_len);

	for (i = 1; i <= loops; i++) {
		/* digest = hash_hmac(salt + pack('N', i), password) { */

		/* pack("N", i) */
		computed_salt[salt_len] = (unsigned char) (i >> 24);
		computed_salt[salt_len + 1] = (unsigned char) ((i & 0xFF0000) >> 16);
		computed_salt[salt_len + 2] = (unsigned char) ((i & 0xFF00) >> 8);
		computed_salt[salt_len + 3] = (unsigned char) (i & 0xFF);

		jonj_hash_hmac_round(digest, ops, context, K1, computed_salt, (long) salt_len + 4);
		jonj_hash_hmac_round(digest, ops, context, K2, digest, ops->digest_size);
		/* } */

		/* temp = digest */
		memcpy(temp, digest, ops->digest_size);

		/* 
		 * Note that the loop starting at 1 is intentional, since we've already done
		 * the first round of the algorithm.
		 */
		for (j = 1; j < iterations; j++) {
			/* digest = hash_hmac(digest, password) { */
			jonj_hash_hmac_round(digest, ops, context, K1, digest, ops->digest_size);
			jonj_hash_hmac_round(digest, ops, context, K2, digest, ops->digest_size);
			/* } */
			/* temp ^= digest */
			jonj_hash_string_xor(temp, temp, digest, ops->digest_size);
		}
		/* result += temp */
		memcpy(result + ((i - 1) * ops->digest_size), temp, ops->digest_size);
	}
	/* Zero potentially sensitive variables */
	memset(K1, 0, ops->block_size);
	memset(K2, 0, ops->block_size);
	memset(computed_salt, 0, salt_len + 4);
	efree(K1);
	efree(K2);
	efree(computed_salt);
	efree(context);
	efree(digest);
	efree(temp);

	returnval = safe_emalloc(length, 1, 1);
	if (raw_output) {
		memcpy(returnval, result, length);
	} else {
		jonj_hash_bin2hex(returnval, result, digest_length);
	}
	returnval[length] = 0;
	efree(result);
	RETURN_STRINGL(returnval, length, 0);
}
/* }}} */

/* {{{ proto bool hash_equals(string known_string, string user_string)
   Compares two strings using the same time whether they're equal or not.
   A difference in length will leak */
JONJ_FUNCTION(hash_equals)
{
	zval *known_zval, *user_zval;
	char *known_str, *user_str;
	int result = 0, j;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &known_zval, &user_zval) == FAILURE) {
		return;
	}

	/* We only allow comparing string to prevent unexpected results. */
	if (Z_TYPE_P(known_zval) != IS_STRING) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected known_string to be a string, %s given", zend_zval_type_name(known_zval));
		RETURN_FALSE;
	}

	if (Z_TYPE_P(user_zval) != IS_STRING) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected user_string to be a string, %s given", zend_zval_type_name(user_zval));
		RETURN_FALSE;
	}

	if (Z_STRLEN_P(known_zval) != Z_STRLEN_P(user_zval)) {
		RETURN_FALSE;
	}

	known_str = Z_STRVAL_P(known_zval);
	user_str = Z_STRVAL_P(user_zval);

	/* This is security sensitive code. Do not optimize this for speed. */
	for (j = 0; j < Z_STRLEN_P(known_zval); j++) {
		result |= known_str[j] ^ user_str[j];
	}

	RETURN_BOOL(0 == result);
}
/* }}} */

/* Module Housekeeping */

static void jonj_hash_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	jonj_hash_data *hash = (jonj_hash_data*)rsrc->ptr;

	/* Just in case the algo has internally allocated resources */
	if (hash->context) {
		unsigned char *dummy = emalloc(hash->ops->digest_size);
		hash->ops->hash_final(dummy, hash->context);
		efree(dummy);
		efree(hash->context);
	}

	if (hash->key) {
		memset(hash->key, 0, hash->ops->block_size);
		efree(hash->key);
	}
	efree(hash);
}
/* }}} */

#define JONJ_HASH_HAVAL_REGISTER(p,b)	jonj_hash_register_algo("haval" #b "," #p , &jonj_hash_##p##haval##b##_ops);

#ifdef JONJ_MHASH_BC

JONJ_MINFO_FUNCTION(mhash)
{
	jonj_info_print_table_start();
	jonj_info_print_table_row(2, "MHASH support", "Enabled");
	jonj_info_print_table_row(2, "MHASH API Version", "Emulated Support");
	jonj_info_print_table_end();
}

zend_module_entry mhash_module_entry = {
	STANDARD_MODULE_HEADER,
	"mhash",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	JONJ_MINFO(mhash),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES,
};

static void mhash_init(INIT_FUNC_ARGS)
{
	char buf[128];
	int len;
	int algo_number = 0;

	for (algo_number = 0; algo_number < MHASH_NUM_ALGOS; algo_number++) {
		struct mhash_bc_entry algorithm = mhash_to_hash[algo_number];
		if (algorithm.mhash_name == NULL) {
			continue;
		}

		len = slprintf(buf, 127, "MHASH_%s", algorithm.mhash_name, strlen(algorithm.mhash_name));
		zend_register_long_constant(buf, len + 1, algorithm.value, CONST_CS | CONST_PERSISTENT, module_number TSRMLS_CC);
	}
	zend_register_internal_module(&mhash_module_entry TSRMLS_CC);
}

/* {{{ proto string mhash(int hash, string data [, string key])
   Hash data with hash */
JONJ_FUNCTION(mhash)
{
	zval **z_algorithm;
	long algorithm;

	if (zend_parse_parameters(1 TSRMLS_CC, "Z", &z_algorithm) == FAILURE) {
		return;
	}

	SEPARATE_ZVAL(z_algorithm);
	convert_to_long_ex(z_algorithm);
	algorithm = Z_LVAL_PP(z_algorithm);

	/* need to convert the first parameter from int constant to string algorithm name */
	if (algorithm >= 0 && algorithm < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.hash_name) {
			ZVAL_STRING(*z_algorithm, algorithm_lookup.hash_name, 1);
		}
	}

	if (ZEND_NUM_ARGS() == 3) {
		jonj_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
	} else if (ZEND_NUM_ARGS() == 2) {
		jonj_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto string mhash_get_hash_name(int hash)
   Gets the name of hash */
JONJ_FUNCTION(mhash_get_hash_name)
{
	long algorithm;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &algorithm) == FAILURE) {
		return;
	}

	if (algorithm >= 0 && algorithm  < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			RETURN_STRING(algorithm_lookup.mhash_name, 1);
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int mhash_count(void)
   Gets the number of available hashes */
JONJ_FUNCTION(mhash_count)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_LONG(MHASH_NUM_ALGOS - 1);
}
/* }}} */

/* {{{ proto int mhash_get_block_size(int hash)
   Gets the block size of hash */
JONJ_FUNCTION(mhash_get_block_size)
{
	long algorithm;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &algorithm) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (algorithm >= 0 && algorithm  < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			const jonj_hash_ops *ops = jonj_hash_fetch_ops(algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name));
			if (ops) {
				RETVAL_LONG(ops->digest_size);
			}
		}
	}
}
/* }}} */

#define SALT_SIZE 8

/* {{{ proto string mhash_keygen_s2k(int hash, string input_password, string salt, int bytes)
   Generates a key using hash functions */
JONJ_FUNCTION(mhash_keygen_s2k)
{
	long algorithm, l_bytes;
	int bytes;
	char *password, *salt;
	int password_len, salt_len;
	char padded_salt[SALT_SIZE];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssl", &algorithm, &password, &password_len, &salt, &salt_len, &l_bytes) == FAILURE) {
		return;
	}

	bytes = (int)l_bytes;
	if (bytes <= 0){
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "the byte parameter must be greater than 0");
		RETURN_FALSE;
	}

	salt_len = MIN(salt_len, SALT_SIZE);

	memcpy(padded_salt, salt, salt_len);
	if (salt_len < SALT_SIZE) {
		memset(padded_salt + salt_len, 0, SALT_SIZE - salt_len);
	}
	salt_len = SALT_SIZE;

	RETVAL_FALSE;
	if (algorithm >= 0 && algorithm < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			const jonj_hash_ops *ops = jonj_hash_fetch_ops(algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name));
			if (ops) {
				unsigned char null = '\0';
				void *context;
				char *key, *digest;
				int i = 0, j = 0;
				int block_size = ops->digest_size;
				int times = bytes / block_size;
				if (bytes % block_size  != 0) times++;

				context = emalloc(ops->context_size);
				ops->hash_init(context);

				key = ecalloc(1, times * block_size);
				digest = emalloc(ops->digest_size + 1);

				for (i = 0; i < times; i++) {
					ops->hash_init(context);

					for (j=0;j<i;j++) {
						ops->hash_update(context, &null, 1);
					}
					ops->hash_update(context, (unsigned char *)padded_salt, salt_len);
					ops->hash_update(context, (unsigned char *)password, password_len);
					ops->hash_final((unsigned char *)digest, context);
					memcpy( &key[i*block_size], digest, block_size);
				}

				RETVAL_STRINGL(key, bytes, 1);
				memset(key, 0, bytes);
				efree(digest);
				efree(context);
				efree(key);
			}
		}
	}
}
/* }}} */

#endif

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(hash)
{
	jonj_hash_le_hash = zend_register_list_destructors_ex(jonj_hash_dtor, NULL, JONJ_HASH_RESNAME, module_number);

	zend_hash_init(&jonj_hash_hashtable, 35, NULL, NULL, 1);

	jonj_hash_register_algo("md2",			&jonj_hash_md2_ops);
	jonj_hash_register_algo("md4",			&jonj_hash_md4_ops);
	jonj_hash_register_algo("md5",			&jonj_hash_md5_ops);
	jonj_hash_register_algo("sha1",			&jonj_hash_sha1_ops);
	jonj_hash_register_algo("sha224",		&jonj_hash_sha224_ops);
	jonj_hash_register_algo("sha256",		&jonj_hash_sha256_ops);
	jonj_hash_register_algo("sha384",		&jonj_hash_sha384_ops);
	jonj_hash_register_algo("sha512",		&jonj_hash_sha512_ops);
	jonj_hash_register_algo("ripemd128",		&jonj_hash_ripemd128_ops);
	jonj_hash_register_algo("ripemd160",		&jonj_hash_ripemd160_ops);
	jonj_hash_register_algo("ripemd256",		&jonj_hash_ripemd256_ops);
	jonj_hash_register_algo("ripemd320",		&jonj_hash_ripemd320_ops);
	jonj_hash_register_algo("whirlpool",		&jonj_hash_whirlpool_ops);
	jonj_hash_register_algo("tiger128,3",	&jonj_hash_3tiger128_ops);
	jonj_hash_register_algo("tiger160,3",	&jonj_hash_3tiger160_ops);
	jonj_hash_register_algo("tiger192,3",	&jonj_hash_3tiger192_ops);
	jonj_hash_register_algo("tiger128,4",	&jonj_hash_4tiger128_ops);
	jonj_hash_register_algo("tiger160,4",	&jonj_hash_4tiger160_ops);
	jonj_hash_register_algo("tiger192,4",	&jonj_hash_4tiger192_ops);
	jonj_hash_register_algo("snefru",		&jonj_hash_snefru_ops);
	jonj_hash_register_algo("snefru256",		&jonj_hash_snefru_ops);
	jonj_hash_register_algo("gost",			&jonj_hash_gost_ops);
	jonj_hash_register_algo("gost-crypto",		&jonj_hash_gost_crypto_ops);
	jonj_hash_register_algo("adler32",		&jonj_hash_adler32_ops);
	jonj_hash_register_algo("crc32",			&jonj_hash_crc32_ops);
	jonj_hash_register_algo("crc32b",		&jonj_hash_crc32b_ops);
	jonj_hash_register_algo("fnv132",		&jonj_hash_fnv132_ops);
	jonj_hash_register_algo("fnv1a32",		&jonj_hash_fnv1a32_ops);
	jonj_hash_register_algo("fnv164",		&jonj_hash_fnv164_ops);
	jonj_hash_register_algo("fnv1a64",		&jonj_hash_fnv1a64_ops);
	jonj_hash_register_algo("joaat",			&jonj_hash_joaat_ops);

	JONJ_HASH_HAVAL_REGISTER(3,128);
	JONJ_HASH_HAVAL_REGISTER(3,160);
	JONJ_HASH_HAVAL_REGISTER(3,192);
	JONJ_HASH_HAVAL_REGISTER(3,224);
	JONJ_HASH_HAVAL_REGISTER(3,256);

	JONJ_HASH_HAVAL_REGISTER(4,128);
	JONJ_HASH_HAVAL_REGISTER(4,160);
	JONJ_HASH_HAVAL_REGISTER(4,192);
	JONJ_HASH_HAVAL_REGISTER(4,224);
	JONJ_HASH_HAVAL_REGISTER(4,256);

	JONJ_HASH_HAVAL_REGISTER(5,128);
	JONJ_HASH_HAVAL_REGISTER(5,160);
	JONJ_HASH_HAVAL_REGISTER(5,192);
	JONJ_HASH_HAVAL_REGISTER(5,224);
	JONJ_HASH_HAVAL_REGISTER(5,256);

	REGISTER_LONG_CONSTANT("HASH_HMAC",		JONJ_HASH_HMAC,	CONST_CS | CONST_PERSISTENT);

#ifdef JONJ_MHASH_BC
	mhash_init(INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(hash)
{
	zend_hash_destroy(&jonj_hash_hashtable);

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(hash)
{
	HashPosition pos;
	char buffer[2048];
	char *s = buffer, *e = s + sizeof(buffer), *str;
	ulong idx;
	long type;

	for(zend_hash_internal_pointer_reset_ex(&jonj_hash_hashtable, &pos);
		(type = zend_hash_get_current_key_ex(&jonj_hash_hashtable, &str, NULL, &idx, 0, &pos)) != HASH_KEY_NON_EXISTENT;
		zend_hash_move_forward_ex(&jonj_hash_hashtable, &pos)) {
		s += slprintf(s, e - s, "%s ", str);
	}
	*s = 0;

	jonj_info_print_table_start();
	jonj_info_print_table_row(2, "hash support", "enabled");
	jonj_info_print_table_row(2, "Hashing Engines", buffer);
	jonj_info_print_table_end();
}
/* }}} */

/* {{{ arginfo */
#ifdef JONJ_HASH_MD5_NOT_IN_CORE
ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_md5, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_md5_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()
#endif

#ifdef JONJ_HASH_SHA1_NOT_IN_CORE
ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_sha1, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_sha1_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash, 0, 0, 2)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_file, 0, 0, 2)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_hmac, 0, 0, 3)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_hmac_file, 0, 0, 3)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_init, 0, 0, 1)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_update, 0)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_update_stream, 0, 0, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_update_file, 0, 0, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_final, 0, 0, 1)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_copy, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_algos, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_pbkdf2, 0, 0, 4)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, salt)
	ZEND_ARG_INFO(0, iterations)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_equals, 0)
	ZEND_ARG_INFO(0, known_string)
	ZEND_ARG_INFO(0, user_string)
ZEND_END_ARG_INFO()

/* BC Land */
#ifdef JONJ_MHASH_BC
ZEND_BEGIN_ARG_INFO(arginfo_mhash_get_block_size, 0)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mhash_get_hash_name, 0)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mhash_keygen_s2k, 0)
	ZEND_ARG_INFO(0, hash)
	ZEND_ARG_INFO(0, input_password)
	ZEND_ARG_INFO(0, salt)
	ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mhash_count, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mhash, 0, 0, 2)
	ZEND_ARG_INFO(0, hash)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
#endif

/* }}} */

/* {{{ hash_functions[]
 */
const zend_function_entry hash_functions[] = {
	JONJ_FE(hash,									arginfo_hash)
	JONJ_FE(hash_file,								arginfo_hash_file)

	JONJ_FE(hash_hmac,								arginfo_hash_hmac)
	JONJ_FE(hash_hmac_file,							arginfo_hash_hmac_file)

	JONJ_FE(hash_init,								arginfo_hash_init)
	JONJ_FE(hash_update,								arginfo_hash_update)
	JONJ_FE(hash_update_stream,						arginfo_hash_update_stream)
	JONJ_FE(hash_update_file,						arginfo_hash_update_file)
	JONJ_FE(hash_final,								arginfo_hash_final)
	JONJ_FE(hash_copy,								arginfo_hash_copy)

	JONJ_FE(hash_algos,								arginfo_hash_algos)
	JONJ_FE(hash_pbkdf2,								arginfo_hash_pbkdf2)
	JONJ_FE(hash_equals,								arginfo_hash_equals)

	/* BC Land */
#ifdef JONJ_HASH_MD5_NOT_IN_CORE
	JONJ_NAMED_FE(md5, jonj_if_md5,					arginfo_hash_md5)
	JONJ_NAMED_FE(md5_file, jonj_if_md5_file,			arginfo_hash_md5_file)
#endif /* JONJ_HASH_MD5_NOT_IN_CORE */

#ifdef JONJ_HASH_SHA1_NOT_IN_CORE
	JONJ_NAMED_FE(sha1, jonj_if_sha1,					arginfo_hash_sha1)
	JONJ_NAMED_FE(sha1_file, jonj_if_sha1_file,		arginfo_hash_sha1_file)
#endif /* JONJ_HASH_SHA1_NOT_IN_CORE */

#ifdef JONJ_MHASH_BC
	JONJ_FE(mhash_keygen_s2k, arginfo_mhash_keygen_s2k)
	JONJ_FE(mhash_get_block_size, arginfo_mhash_get_block_size)
	JONJ_FE(mhash_get_hash_name, arginfo_mhash_get_hash_name)
	JONJ_FE(mhash_count, arginfo_mhash_count)
	JONJ_FE(mhash, arginfo_mhash)
#endif

	JONJ_FE_END
};
/* }}} */

/* {{{ hash_module_entry
 */
zend_module_entry hash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	JONJ_HASH_EXTNAME,
	hash_functions,
	JONJ_MINIT(hash),
	JONJ_MSHUTDOWN(hash),
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	JONJ_MINFO(hash),
#if ZEND_MODULE_API_NO >= 20010901
	JONJ_HASH_EXTVER, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HASH
ZEND_GET_MODULE(hash)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

