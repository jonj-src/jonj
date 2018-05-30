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
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Memory Mapping interface for streams.
 * The intention is to provide a uniform interface over the most common
 * operations that are used within JONJ itself, rather than a complete
 * API for all memory mapping needs.
 *
 * ATM, we support only mmap(), but win32 memory mapping support will
 * follow soon.
 * */

typedef enum {
	/* Does the stream support mmap ? */
	JONJ_STREAM_MMAP_SUPPORTED,
	/* Request a range and offset to be mapped;
	 * while mapped, you MUST NOT use any read/write functions
	 * on the stream (win9x compatibility) */
	JONJ_STREAM_MMAP_MAP_RANGE,
	/* Unmap the last range that was mapped for the stream */
	JONJ_STREAM_MMAP_UNMAP
} jonj_stream_mmap_operation_t;
	
typedef enum {
	JONJ_STREAM_MAP_MODE_READONLY,
	JONJ_STREAM_MAP_MODE_READWRITE,
	JONJ_STREAM_MAP_MODE_SHARED_READONLY,
	JONJ_STREAM_MAP_MODE_SHARED_READWRITE
} jonj_stream_mmap_access_t;

typedef struct {
	/* requested offset and length.
	 * If length is 0, the whole file is mapped */
	size_t offset;
	size_t length;
	
	jonj_stream_mmap_access_t mode;
	
	/* returned mapped address */
	char *mapped;

} jonj_stream_mmap_range;

#define JONJ_STREAM_MMAP_ALL 0

#define jonj_stream_mmap_supported(stream)	(_jonj_stream_set_option((stream), JONJ_STREAM_OPTION_MMAP_API, JONJ_STREAM_MMAP_SUPPORTED, NULL TSRMLS_CC) == 0 ? 1 : 0)

/* Returns 1 if the stream in its current state can be memory mapped,
 * 0 otherwise */
#define jonj_stream_mmap_possible(stream)			(!jonj_stream_is_filtered((stream)) && jonj_stream_mmap_supported((stream)))

BEGIN_EXTERN_C()
JONJAPI char *_jonj_stream_mmap_range(jonj_stream *stream, size_t offset, size_t length, jonj_stream_mmap_operation_t mode, size_t *mapped_len TSRMLS_DC);
#define jonj_stream_mmap_range(stream, offset, length, mode, mapped_len)	_jonj_stream_mmap_range((stream), (offset), (length), (mode), (mapped_len) TSRMLS_CC)

/* un-maps the last mapped range */
JONJAPI int _jonj_stream_mmap_unmap(jonj_stream *stream TSRMLS_DC);
#define jonj_stream_mmap_unmap(stream)				_jonj_stream_mmap_unmap((stream) TSRMLS_CC)

JONJAPI int _jonj_stream_mmap_unmap_ex(jonj_stream *stream, off_t readden TSRMLS_DC);
#define jonj_stream_mmap_unmap_ex(stream, readden)			_jonj_stream_mmap_unmap_ex((stream), (readden) TSRMLS_CC)
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
