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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   | With suggestions from:                                               |
   |      Moriyoshi Koizumi <moriyoshi@at.wakwak.com>                     |
   |      Sara Golemon      <pollita@jonj.tk>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* The filter API works on the principle of "Bucket-Brigades".  This is
 * partially inspired by the Apache 2 method of doing things, although
 * it is intentially a light-weight implementation.
 *
 * Each stream can have a chain of filters for reading and another for writing.
 * 
 * When data is written to the stream, it is placed into a bucket and placed at
 * the start of the input brigade.
 *
 * The first filter in the chain is invoked on the brigade and (depending on
 * it's return value), the next filter is invoked and so on.
 * */

#define JONJ_STREAM_FILTER_READ	0x0001
#define JONJ_STREAM_FILTER_WRITE	0x0002
#define JONJ_STREAM_FILTER_ALL	(JONJ_STREAM_FILTER_READ | JONJ_STREAM_FILTER_WRITE)

typedef struct _jonj_stream_bucket			jonj_stream_bucket;
typedef struct _jonj_stream_bucket_brigade	jonj_stream_bucket_brigade;

struct _jonj_stream_bucket {
	jonj_stream_bucket *next, *prev;
	jonj_stream_bucket_brigade *brigade;

	char *buf;
	size_t buflen;
	/* if non-zero, buf should be pefreed when the bucket is destroyed */
	int own_buf;
	int is_persistent;
	
	/* destroy this struct when refcount falls to zero */
	int refcount;
};

struct _jonj_stream_bucket_brigade {
	jonj_stream_bucket *head, *tail;
};

typedef enum {
	PSFS_ERR_FATAL,	/* error in data stream */
	PSFS_FEED_ME,	/* filter needs more data; stop processing chain until more is available */
	PSFS_PASS_ON	/* filter generated output buckets; pass them on to next in chain */
} jonj_stream_filter_status_t;

/* Buckets API. */
BEGIN_EXTERN_C()
JONJAPI jonj_stream_bucket *jonj_stream_bucket_new(jonj_stream *stream, char *buf, size_t buflen, int own_buf, int buf_persistent TSRMLS_DC);
JONJAPI int jonj_stream_bucket_split(jonj_stream_bucket *in, jonj_stream_bucket **left, jonj_stream_bucket **right, size_t length TSRMLS_DC);
JONJAPI void jonj_stream_bucket_delref(jonj_stream_bucket *bucket TSRMLS_DC);
#define jonj_stream_bucket_addref(bucket)	(bucket)->refcount++
JONJAPI void jonj_stream_bucket_prepend(jonj_stream_bucket_brigade *brigade, jonj_stream_bucket *bucket TSRMLS_DC);
JONJAPI void jonj_stream_bucket_append(jonj_stream_bucket_brigade *brigade, jonj_stream_bucket *bucket TSRMLS_DC);
JONJAPI void jonj_stream_bucket_unlink(jonj_stream_bucket *bucket TSRMLS_DC);
JONJAPI jonj_stream_bucket *jonj_stream_bucket_make_writeable(jonj_stream_bucket *bucket TSRMLS_DC);
END_EXTERN_C()

#define PSFS_FLAG_NORMAL		0	/* regular read/write */
#define PSFS_FLAG_FLUSH_INC		1	/* an incremental flush */
#define PSFS_FLAG_FLUSH_CLOSE	2	/* final flush prior to closing */

typedef struct _jonj_stream_filter_ops {

	jonj_stream_filter_status_t (*filter)(
			jonj_stream *stream,
			jonj_stream_filter *thisfilter,
			jonj_stream_bucket_brigade *buckets_in,
			jonj_stream_bucket_brigade *buckets_out,
			size_t *bytes_consumed,
			int flags
			TSRMLS_DC);
	
	void (*dtor)(jonj_stream_filter *thisfilter TSRMLS_DC);
	
	const char *label;
	
} jonj_stream_filter_ops;

typedef struct _jonj_stream_filter_chain {
	jonj_stream_filter *head, *tail;

	/* Owning stream */
	jonj_stream *stream;
} jonj_stream_filter_chain;

struct _jonj_stream_filter {
	jonj_stream_filter_ops *fops;
	void *abstract; /* for use by filter implementation */
	jonj_stream_filter *next;
	jonj_stream_filter *prev;
	int is_persistent;

	/* link into stream and chain */
	jonj_stream_filter_chain *chain;

	/* buffered buckets */
	jonj_stream_bucket_brigade buffer;

	/* filters are auto_registered when they're applied */
	int rsrc_id;
};

/* stack filter onto a stream */
BEGIN_EXTERN_C()
JONJAPI void _jonj_stream_filter_prepend(jonj_stream_filter_chain *chain, jonj_stream_filter *filter TSRMLS_DC);
JONJAPI int jonj_stream_filter_prepend_ex(jonj_stream_filter_chain *chain, jonj_stream_filter *filter TSRMLS_DC);
JONJAPI void _jonj_stream_filter_append(jonj_stream_filter_chain *chain, jonj_stream_filter *filter TSRMLS_DC);
JONJAPI int jonj_stream_filter_append_ex(jonj_stream_filter_chain *chain, jonj_stream_filter *filter TSRMLS_DC);
JONJAPI int _jonj_stream_filter_flush(jonj_stream_filter *filter, int finish TSRMLS_DC);
JONJAPI jonj_stream_filter *jonj_stream_filter_remove(jonj_stream_filter *filter, int call_dtor TSRMLS_DC);
JONJAPI void jonj_stream_filter_free(jonj_stream_filter *filter TSRMLS_DC);
JONJAPI jonj_stream_filter *_jonj_stream_filter_alloc(jonj_stream_filter_ops *fops, void *abstract, int persistent STREAMS_DC TSRMLS_DC);
END_EXTERN_C()
#define jonj_stream_filter_alloc(fops, thisptr, persistent) _jonj_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_CC TSRMLS_CC)
#define jonj_stream_filter_alloc_rel(fops, thisptr, persistent) _jonj_stream_filter_alloc((fops), (thisptr), (persistent) STREAMS_REL_CC TSRMLS_CC)
#define jonj_stream_filter_prepend(chain, filter) _jonj_stream_filter_prepend((chain), (filter) TSRMLS_CC)
#define jonj_stream_filter_append(chain, filter) _jonj_stream_filter_append((chain), (filter) TSRMLS_CC)
#define jonj_stream_filter_flush(filter, finish) _jonj_stream_filter_flush((filter), (finish) TSRMLS_CC)

#define jonj_stream_is_filtered(stream)	((stream)->readfilters.head || (stream)->writefilters.head)

typedef struct _jonj_stream_filter_factory {
	jonj_stream_filter *(*create_filter)(const char *filtername, zval *filterparams, int persistent TSRMLS_DC);
} jonj_stream_filter_factory;

BEGIN_EXTERN_C()
JONJAPI int jonj_stream_filter_register_factory(const char *filterpattern, jonj_stream_filter_factory *factory TSRMLS_DC);
JONJAPI int jonj_stream_filter_unregister_factory(const char *filterpattern TSRMLS_DC);
JONJAPI int jonj_stream_filter_register_factory_volatile(const char *filterpattern, jonj_stream_filter_factory *factory TSRMLS_DC);
JONJAPI jonj_stream_filter *jonj_stream_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC);
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
