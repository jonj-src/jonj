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
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   | Sara Golemon (pollita@jonj.tk)                                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj.h"
#include "jonj_globals.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"

#define JONJ_STREAM_BRIGADE_RES_NAME	"userfilter.bucket brigade"
#define JONJ_STREAM_BUCKET_RES_NAME "userfilter.bucket"
#define JONJ_STREAM_FILTER_RES_NAME "userfilter.filter"

struct jonj_user_filter_data {
	zend_class_entry *ce;
	/* variable length; this *must* be last in the structure */
	char classname[1];
};

/* to provide context for calling into the next filter from user-space */
static int le_userfilters;
static int le_bucket_brigade;
static int le_bucket;

#define GET_FILTER_FROM_OBJ()	{ \
	zval **tmp; \
	if (FAILURE == zend_hash_index_find(Z_OBJPROP_P(this_ptr), 0, (void**)&tmp)) { \
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "filter property vanished"); \
		RETURN_FALSE; \
	} \
	ZEND_FETCH_RESOURCE(filter, jonj_stream_filter*, tmp, -1, "filter", le_userfilters); \
}

/* define the base filter class */

JONJ_FUNCTION(user_filter_nop)
{
}
ZEND_BEGIN_ARG_INFO(arginfo_jonj_user_filter_filter, 0)
	ZEND_ARG_INFO(0, in)
	ZEND_ARG_INFO(0, out)
	ZEND_ARG_INFO(1, consumed)
	ZEND_ARG_INFO(0, closing)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_jonj_user_filter_onCreate, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_jonj_user_filter_onClose, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry user_filter_class_funcs[] = {
	JONJ_NAMED_FE(filter,	JONJ_FN(user_filter_nop),		arginfo_jonj_user_filter_filter)
	JONJ_NAMED_FE(onCreate,	JONJ_FN(user_filter_nop),		arginfo_jonj_user_filter_onCreate)
	JONJ_NAMED_FE(onClose,	JONJ_FN(user_filter_nop),		arginfo_jonj_user_filter_onClose)
	JONJ_FE_END
};

static zend_class_entry user_filter_class_entry;

static ZEND_RSRC_DTOR_FUNC(jonj_bucket_dtor)
{
	jonj_stream_bucket *bucket = (jonj_stream_bucket *)rsrc->ptr;
	if (bucket) {
		jonj_stream_bucket_delref(bucket TSRMLS_CC);
		bucket = NULL;
	}
}

JONJ_MINIT_FUNCTION(user_filters)
{
	zend_class_entry *jonj_user_filter;
	/* init the filter class ancestor */
	INIT_CLASS_ENTRY(user_filter_class_entry, "jonj_user_filter", user_filter_class_funcs);
	if ((jonj_user_filter = zend_register_internal_class(&user_filter_class_entry TSRMLS_CC)) == NULL) {
		return FAILURE;
	}
	zend_declare_property_string(jonj_user_filter, "filtername", sizeof("filtername")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(jonj_user_filter, "params", sizeof("params")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);

	/* init the filter resource; it has no dtor, as streams will always clean it up
	 * at the correct time */
	le_userfilters = zend_register_list_destructors_ex(NULL, NULL, JONJ_STREAM_FILTER_RES_NAME, 0);

	if (le_userfilters == FAILURE) {
		return FAILURE;
	}

	/* Filters will dispose of their brigades */
	le_bucket_brigade = zend_register_list_destructors_ex(NULL, NULL, JONJ_STREAM_BRIGADE_RES_NAME, module_number);
	/* Brigades will dispose of their buckets */
	le_bucket = zend_register_list_destructors_ex(jonj_bucket_dtor, NULL, JONJ_STREAM_BUCKET_RES_NAME, module_number);
	
	if (le_bucket_brigade == FAILURE) {
		return FAILURE;
	}

	REGISTER_LONG_CONSTANT("PSFS_PASS_ON",			PSFS_PASS_ON,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_FEED_ME",			PSFS_FEED_ME,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_ERR_FATAL",		PSFS_ERR_FATAL,			CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PSFS_FLAG_NORMAL",		PSFS_FLAG_NORMAL,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_FLAG_FLUSH_INC",	PSFS_FLAG_FLUSH_INC,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PSFS_FLAG_FLUSH_CLOSE",	PSFS_FLAG_FLUSH_CLOSE,	CONST_CS | CONST_PERSISTENT);
	
	return SUCCESS;
}

JONJ_RSHUTDOWN_FUNCTION(user_filters)
{
	if (BG(user_filter_map)) {
		zend_hash_destroy(BG(user_filter_map));
		efree(BG(user_filter_map));
		BG(user_filter_map) = NULL;
	}

	return SUCCESS;
}

static void userfilter_dtor(jonj_stream_filter *thisfilter TSRMLS_DC)
{
	zval *obj = (zval*)thisfilter->abstract;
	zval func_name;
	zval *retval = NULL;

	if (obj == NULL) {
		/* If there's no object associated then there's nothing to dispose of */
		return;
	}

	ZVAL_STRINGL(&func_name, "onclose", sizeof("onclose")-1, 0);

	call_user_function_ex(NULL,
			&obj,
			&func_name,
			&retval,
			0, NULL,
			0, NULL TSRMLS_CC);

	if (retval)
		zval_ptr_dtor(&retval);

	/* kill the object */
	zval_ptr_dtor(&obj);
}

jonj_stream_filter_status_t userfilter_filter(
			jonj_stream *stream,
			jonj_stream_filter *thisfilter,
			jonj_stream_bucket_brigade *buckets_in,
			jonj_stream_bucket_brigade *buckets_out,
			size_t *bytes_consumed,
			int flags
			TSRMLS_DC)
{
	int ret = PSFS_ERR_FATAL;
	zval *obj = (zval*)thisfilter->abstract;
	zval func_name;
	zval *retval = NULL;
	zval **args[4];
	zval *zclosing, *zconsumed, *zin, *zout, *zstream;
	zval zpropname;
	int call_result;

	/* the userfilter object probably doesn't exist anymore */
	if (CG(unclean_shutdown)) {
		return ret;
	}

	if (FAILURE == zend_hash_find(Z_OBJPROP_P(obj), "stream", sizeof("stream"), (void**)&zstream)) {
		/* Give the userfilter class a hook back to the stream */
		ALLOC_INIT_ZVAL(zstream);
		jonj_stream_to_zval(stream, zstream);
		zval_copy_ctor(zstream);
		add_property_zval(obj, "stream", zstream);
		/* add_property_zval increments the refcount which is unwanted here */
		zval_ptr_dtor(&zstream);
	}

	ZVAL_STRINGL(&func_name, "filter", sizeof("filter")-1, 0);

	/* Setup calling arguments */
	ALLOC_INIT_ZVAL(zin);
	ZEND_REGISTER_RESOURCE(zin, buckets_in, le_bucket_brigade);
	args[0] = &zin;

	ALLOC_INIT_ZVAL(zout);
	ZEND_REGISTER_RESOURCE(zout, buckets_out, le_bucket_brigade);
	args[1] = &zout;

	ALLOC_INIT_ZVAL(zconsumed);
	if (bytes_consumed) {
		ZVAL_LONG(zconsumed, *bytes_consumed);
	} else {
		ZVAL_NULL(zconsumed);
	}
	args[2] = &zconsumed;

	ALLOC_INIT_ZVAL(zclosing);
	ZVAL_BOOL(zclosing, flags & PSFS_FLAG_FLUSH_CLOSE);
	args[3] = &zclosing;

	call_result = call_user_function_ex(NULL,
			&obj,
			&func_name,
			&retval,
			4, args,
			0, NULL TSRMLS_CC);

	if (call_result == SUCCESS && retval != NULL) {
		convert_to_long(retval);
		ret = Z_LVAL_P(retval);
	} else if (call_result == FAILURE) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to call filter function");
	}

	if (bytes_consumed) {
		*bytes_consumed = Z_LVAL_P(zconsumed);
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	if (buckets_in->head) {
		jonj_stream_bucket *bucket = buckets_in->head;

		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unprocessed filter buckets remaining on input brigade");
		while ((bucket = buckets_in->head)) {
			/* Remove unconsumed buckets from the brigade */
			jonj_stream_bucket_unlink(bucket TSRMLS_CC);
			jonj_stream_bucket_delref(bucket TSRMLS_CC);
		}
	}
	if (ret != PSFS_PASS_ON) {
		jonj_stream_bucket *bucket = buckets_out->head;
		while (bucket != NULL) {
			jonj_stream_bucket_unlink(bucket TSRMLS_CC);
			jonj_stream_bucket_delref(bucket TSRMLS_CC);
			bucket = buckets_out->head;
		}
	}

	/* filter resources are cleaned up by the stream destructor,
	 * keeping a reference to the stream resource here would prevent it
	 * from being destroyed properly */
	INIT_ZVAL(zpropname);
	ZVAL_STRINGL(&zpropname, "stream", sizeof("stream")-1, 0);
	Z_OBJ_HANDLER_P(obj, unset_property)(obj, &zpropname, 0 TSRMLS_CC);

	zval_ptr_dtor(&zclosing);
	zval_ptr_dtor(&zconsumed);
	zval_ptr_dtor(&zout);
	zval_ptr_dtor(&zin);

	return ret;
}

static jonj_stream_filter_ops userfilter_ops = {
	userfilter_filter,
	userfilter_dtor,
	"user-filter"
};

static jonj_stream_filter *user_filter_factory_create(const char *filtername,
		zval *filterparams, int persistent TSRMLS_DC)
{
	struct jonj_user_filter_data *fdat = NULL;
	jonj_stream_filter *filter;
	zval *obj, *zfilter;
	zval func_name;
	zval *retval = NULL;
	int len;
	
	/* some sanity checks */
	if (persistent) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING,
				"cannot use a user-space filter with a persistent stream");
		return NULL;
	}

	len = strlen(filtername);

	/* determine the classname/class entry */
	if (FAILURE == zend_hash_find(BG(user_filter_map), (char*)filtername, len + 1, (void**)&fdat)) {
		char *period;

		/* Userspace Filters using ambiguous wildcards could cause problems.
           i.e.: myfilter.foo.bar will always call into myfilter.foo.*
                 never seeing myfilter.* 
           TODO: Allow failed userfilter creations to continue
                 scanning through the list */
		if ((period = strrchr(filtername, '.'))) {
			char *wildcard = emalloc(len + 3);

			/* Search for wildcard matches instead */
			memcpy(wildcard, filtername, len + 1); /* copy \0 */
			period = wildcard + (period - filtername);
			while (period) {
				*period = '\0';
				strncat(wildcard, ".*", 2);
				if (SUCCESS == zend_hash_find(BG(user_filter_map), wildcard, strlen(wildcard) + 1, (void**)&fdat)) {
					period = NULL;
				} else {
					*period = '\0';
					period = strrchr(wildcard, '.');
				}
			}
			efree(wildcard);
		}
		if (fdat == NULL) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Err, filter \"%s\" is not in the user-filter map, but somehow the user-filter-factory was invoked for it!?", filtername);
			return NULL;
		}
	}

	/* bind the classname to the actual class */
	if (fdat->ce == NULL) {
		if (FAILURE == zend_lookup_class(fdat->classname, strlen(fdat->classname),
					(zend_class_entry ***)&fdat->ce TSRMLS_CC)) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING,
					"user-filter \"%s\" requires class \"%s\", but that class is not defined",
					filtername, fdat->classname);
			return NULL;
		}
		fdat->ce = *(zend_class_entry**)fdat->ce;

	}

	filter = jonj_stream_filter_alloc(&userfilter_ops, NULL, 0);
	if (filter == NULL) {
		return NULL;
	}

	/* create the object */
	ALLOC_ZVAL(obj);
	object_init_ex(obj, fdat->ce);
	Z_SET_REFCOUNT_P(obj, 1);
	Z_SET_ISREF_P(obj);

	/* filtername */
	add_property_string(obj, "filtername", (char*)filtername, 1);
	
	/* and the parameters, if any */
	if (filterparams) {
		add_property_zval(obj, "params", filterparams);
	} else {
		add_property_null(obj, "params");
	}

	/* invoke the constructor */
	ZVAL_STRINGL(&func_name, "oncreate", sizeof("oncreate")-1, 0);

	call_user_function_ex(NULL,
			&obj,
			&func_name,
			&retval,
			0, NULL,
			0, NULL TSRMLS_CC);

	if (retval) {
		if (Z_TYPE_P(retval) == IS_BOOL && Z_LVAL_P(retval) == 0) {
			/* User reported filter creation error "return false;" */
			zval_ptr_dtor(&retval);

			/* Kill the filter (safely) */
			filter->abstract = NULL;
			jonj_stream_filter_free(filter TSRMLS_CC);

			/* Kill the object */
			zval_ptr_dtor(&obj);

			/* Report failure to filter_alloc */
			return NULL;
		}			
		zval_ptr_dtor(&retval);
	}

	/* set the filter property, this will be used during cleanup */
	ALLOC_INIT_ZVAL(zfilter);
	ZEND_REGISTER_RESOURCE(zfilter, filter, le_userfilters);
	filter->abstract = obj;
	add_property_zval(obj, "filter", zfilter);
	/* add_property_zval increments the refcount which is unwanted here */
	zval_ptr_dtor(&zfilter);

	return filter;
}

static jonj_stream_filter_factory user_filter_factory = {
	user_filter_factory_create
};

static void filter_item_dtor(struct jonj_user_filter_data *fdat)
{
}

/* {{{ proto object stream_bucket_make_writeable(resource brigade)
   Return a bucket object from the brigade for operating on */
JONJ_FUNCTION(stream_bucket_make_writeable)
{
	zval *zbrigade, *zbucket;
	jonj_stream_bucket_brigade *brigade;
	jonj_stream_bucket *bucket;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zbrigade) == FAILURE) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(brigade, jonj_stream_bucket_brigade *, &zbrigade, -1, JONJ_STREAM_BRIGADE_RES_NAME, le_bucket_brigade); 

	ZVAL_NULL(return_value);

	if (brigade->head && (bucket = jonj_stream_bucket_make_writeable(brigade->head TSRMLS_CC))) {
		ALLOC_INIT_ZVAL(zbucket);
		ZEND_REGISTER_RESOURCE(zbucket, bucket, le_bucket);
		object_init(return_value);
		add_property_zval(return_value, "bucket", zbucket);
		/* add_property_zval increments the refcount which is unwanted here */
		zval_ptr_dtor(&zbucket);
		add_property_stringl(return_value, "data", bucket->buf, bucket->buflen, 1);
		add_property_long(return_value, "datalen", bucket->buflen);
	}
}
/* }}} */

/* {{{ jonj_stream_bucket_attach */
static void jonj_stream_bucket_attach(int append, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *zbrigade, *zobject;
	zval **pzbucket, **pzdata;
	jonj_stream_bucket_brigade *brigade;
	jonj_stream_bucket *bucket;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zo", &zbrigade, &zobject) == FAILURE) {
		RETURN_FALSE;
	}

	if (FAILURE == zend_hash_find(Z_OBJPROP_P(zobject), "bucket", 7, (void**)&pzbucket)) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Object has no bucket property");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(brigade, jonj_stream_bucket_brigade *, &zbrigade, -1, JONJ_STREAM_BRIGADE_RES_NAME, le_bucket_brigade);
	ZEND_FETCH_RESOURCE(bucket, jonj_stream_bucket *, pzbucket, -1, JONJ_STREAM_BUCKET_RES_NAME, le_bucket);

	if (SUCCESS == zend_hash_find(Z_OBJPROP_P(zobject), "data", 5, (void**)&pzdata) && (*pzdata)->type == IS_STRING) {
		if (!bucket->own_buf) {
			bucket = jonj_stream_bucket_make_writeable(bucket TSRMLS_CC);
		}
		if ((int)bucket->buflen != Z_STRLEN_PP(pzdata)) {
			bucket->buf = perealloc(bucket->buf, Z_STRLEN_PP(pzdata), bucket->is_persistent);
			bucket->buflen = Z_STRLEN_PP(pzdata);
		}
		memcpy(bucket->buf, Z_STRVAL_PP(pzdata), bucket->buflen);
	}

	if (append) {
		jonj_stream_bucket_append(brigade, bucket TSRMLS_CC);
	} else {
		jonj_stream_bucket_prepend(brigade, bucket TSRMLS_CC);
	}
	/* This is a hack necessary to accommodate situations where bucket is appended to the stream
 	 * multiple times. See bug35916.jonjt for reference.
	 */
	if (bucket->refcount == 1) {
		bucket->refcount++;
	}
}
/* }}} */

/* {{{ proto void stream_bucket_prepend(resource brigade, object bucket)
   Prepend bucket to brigade */
JONJ_FUNCTION(stream_bucket_prepend)
{
	jonj_stream_bucket_attach(0, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void stream_bucket_append(resource brigade, object bucket)
   Append bucket to brigade */
JONJ_FUNCTION(stream_bucket_append)
{
	jonj_stream_bucket_attach(1, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto resource stream_bucket_new(resource stream, string buffer)
   Create a new bucket for use on the current stream */
JONJ_FUNCTION(stream_bucket_new)
{
	zval *zstream, *zbucket;
	jonj_stream *stream;
	char *buffer;
	char *pbuffer;
	int buffer_len;
	jonj_stream_bucket *bucket;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &zstream, &buffer, &buffer_len) == FAILURE) {
		RETURN_FALSE;
	}

	jonj_stream_from_zval(stream, &zstream);

	if (!(pbuffer = pemalloc(buffer_len, jonj_stream_is_persistent(stream)))) {
		RETURN_FALSE;
	}

	memcpy(pbuffer, buffer, buffer_len);

	bucket = jonj_stream_bucket_new(stream, pbuffer, buffer_len, 1, jonj_stream_is_persistent(stream) TSRMLS_CC);
	
	if (bucket == NULL) {
		RETURN_FALSE;
	}

	ALLOC_INIT_ZVAL(zbucket);
	ZEND_REGISTER_RESOURCE(zbucket, bucket, le_bucket);
	object_init(return_value);
	add_property_zval(return_value, "bucket", zbucket);
	/* add_property_zval increments the refcount which is unwanted here */
	zval_ptr_dtor(&zbucket);
	add_property_stringl(return_value, "data", bucket->buf, bucket->buflen, 1);
	add_property_long(return_value, "datalen", bucket->buflen);
}
/* }}} */

/* {{{ proto array stream_get_filters(void)
   Returns a list of registered filters */
JONJ_FUNCTION(stream_get_filters)
{
	char *filter_name;
	int key_flags;
	uint filter_name_len = 0;
	HashTable *filters_hash;
	ulong num_key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	filters_hash = jonj_get_stream_filters_hash();

	if (filters_hash) {
		for(zend_hash_internal_pointer_reset(filters_hash);
			(key_flags = zend_hash_get_current_key_ex(filters_hash, &filter_name, &filter_name_len, &num_key, 0, NULL)) != HASH_KEY_NON_EXISTENT;
			zend_hash_move_forward(filters_hash))
				if (key_flags == HASH_KEY_IS_STRING) {
					add_next_index_stringl(return_value, filter_name, filter_name_len - 1, 1);
				}
	}
	/* It's okay to return an empty array if no filters are registered */
}
/* }}} */	

/* {{{ proto bool stream_filter_register(string filtername, string classname)
   Registers a custom filter handler class */
JONJ_FUNCTION(stream_filter_register)
{
	char *filtername, *classname;
	int filtername_len, classname_len;
	struct jonj_user_filter_data *fdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filtername, &filtername_len,
				&classname, &classname_len) == FAILURE) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	if (!filtername_len) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Filter name cannot be empty");
		return;
	}

	if (!classname_len) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Class name cannot be empty");
		return;
	}

	if (!BG(user_filter_map)) {
		BG(user_filter_map) = (HashTable*) emalloc(sizeof(HashTable));
		zend_hash_init(BG(user_filter_map), 5, NULL, (dtor_func_t) filter_item_dtor, 0);
	}

	fdat = ecalloc(1, sizeof(struct jonj_user_filter_data) + classname_len);
	memcpy(fdat->classname, classname, classname_len);

	if (zend_hash_add(BG(user_filter_map), filtername, filtername_len + 1, (void*)fdat,
				sizeof(*fdat) + classname_len, NULL) == SUCCESS &&
			jonj_stream_filter_register_factory_volatile(filtername, &user_filter_factory TSRMLS_CC) == SUCCESS) {
		RETVAL_TRUE;
	}

	efree(fdat);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
