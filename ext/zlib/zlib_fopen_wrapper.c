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
   | Author: Wez Furlong <wez@thebrainroom.com>, based on work by:        |
   |         Hartmut Holzgraefe <hholzgra@jonj.tk>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define _GNU_SOURCE

#include "jonj.h"
#include "jonj_zlib.h"
#include "fopen_wrappers.h"

#include "main/jonj.tkwork.h"

struct jonj_gz_stream_data_t	{
	gzFile gz_file;
	jonj_stream *stream;
};

static size_t jonj_gziop_read(jonj_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	struct jonj_gz_stream_data_t *self = (struct jonj_gz_stream_data_t *) stream->abstract;
	int read;
	
	read = gzread(self->gz_file, buf, count);
	
	if (gzeof(self->gz_file)) {
		stream->eof = 1;
	}
		
	return (read < 0) ? 0 : read;
}

static size_t jonj_gziop_write(jonj_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	struct jonj_gz_stream_data_t *self = (struct jonj_gz_stream_data_t *) stream->abstract;
	int wrote;

	wrote = gzwrite(self->gz_file, (char *) buf, count);

	return (wrote < 0) ? 0 : wrote;
}

static int jonj_gziop_seek(jonj_stream *stream, off_t offset, int whence, off_t *newoffs TSRMLS_DC)
{
	struct jonj_gz_stream_data_t *self = (struct jonj_gz_stream_data_t *) stream->abstract;

	assert(self != NULL);

	if (whence == SEEK_END) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "SEEK_END is not supported");
		return -1;
	}
	*newoffs = gzseek(self->gz_file, offset, whence);

	return (*newoffs < 0) ? -1 : 0;
}

static int jonj_gziop_close(jonj_stream *stream, int close_handle TSRMLS_DC)
{
	struct jonj_gz_stream_data_t *self = (struct jonj_gz_stream_data_t *) stream->abstract;
	int ret = EOF;
	
	if (close_handle) {
		if (self->gz_file) {
			ret = gzclose(self->gz_file);
			self->gz_file = NULL;
		}
		if (self->stream) {
			jonj_stream_close(self->stream);
			self->stream = NULL;
		}
	}
	efree(self);

	return ret;
}

static int jonj_gziop_flush(jonj_stream *stream TSRMLS_DC)
{
	struct jonj_gz_stream_data_t *self = (struct jonj_gz_stream_data_t *) stream->abstract;

	return gzflush(self->gz_file, Z_SYNC_FLUSH);
}

jonj_stream_ops jonj_stream_gzio_ops = {
	jonj_gziop_write, jonj_gziop_read,
	jonj_gziop_close, jonj_gziop_flush,
	"ZLIB",
	jonj_gziop_seek, 
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

jonj_stream *jonj_stream_gzopen(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, 
							  char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC)
{
	struct jonj_gz_stream_data_t *self;
	jonj_stream *stream = NULL, *innerstream = NULL;

	/* sanity check the stream: it can be either read-only or write-only */
	if (strchr(mode, '+')) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot open a zlib stream for reading and writing at the same time!");
		}
		return NULL;
	}
	
	if (strncasecmp("compress.zlib://", path, 16) == 0) {
		path += 16;
	} else if (strncasecmp("zlib:", path, 5) == 0) {
		path += 5;
	}
	
	innerstream = jonj_stream_open_wrapper_ex(path, mode, STREAM_MUST_SEEK | options | STREAM_WILL_CAST, opened_path, context);
	
	if (innerstream) {
		jonj_socket_t fd;

		if (SUCCESS == jonj_stream_cast(innerstream, JONJ_STREAM_AS_FD, (void **) &fd, REPORT_ERRORS)) {
			self = emalloc(sizeof(*self));
			self->stream = innerstream;
			self->gz_file = gzdopen(dup(fd), mode);

			if (self->gz_file) {
				stream = jonj_stream_alloc_rel(&jonj_stream_gzio_ops, self, 0, mode);
				if (stream) {
					stream->flags |= JONJ_STREAM_FLAG_NO_BUFFER;
					return stream;
				}

				gzclose(self->gz_file);
			}

			efree(self);
			if (options & REPORT_ERRORS) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "gzopen failed");
			}
		}

		jonj_stream_close(innerstream);
	}

	return NULL;
}

static jonj_stream_wrapper_ops gzip_stream_wops = {
	jonj_stream_gzopen,
	NULL, /* close */
	NULL, /* stat */
	NULL, /* stat_url */
	NULL, /* opendir */
	"ZLIB",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};

jonj_stream_wrapper jonj_stream_gzip_wrapper =	{
	&gzip_stream_wops,
	NULL,
	0, /* is_url */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
