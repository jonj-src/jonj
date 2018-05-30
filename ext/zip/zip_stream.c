/*
  +----------------------------------------------------------------------+
  | JONJ Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the JONJ license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_01.txt.                                 |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Piere-Alain Joye <pierre@jonj.tk>                            |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif
#include "jonj.h"
#if HAVE_ZIP
#ifdef ZEND_ENGINE_2

#include "jonj_streams.h"
#include "ext/standard/file.h"
#include "ext/standard/jonj_string.h"
#include "fopen_wrappers.h"
#include "jonj_zip.h"

#include "ext/standard/url.h"

struct jonj_zip_stream_data_t {
	struct zip *za;
	struct zip_file *zf;
	size_t cursor;
	jonj_stream *stream;
};

#define STREAM_DATA_FROM_STREAM() \
	struct jonj_zip_stream_data_t *self = (struct jonj_zip_stream_data_t *) stream->abstract;


/* {{{ jonj_zip_ops_read */
static size_t jonj_zip_ops_read(jonj_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	ssize_t n = 0;
	STREAM_DATA_FROM_STREAM();

	if (self->za && self->zf) {
		n = zip_fread(self->zf, buf, count);
		if (n < 0) {
			int ze, se;
			zip_file_error_get(self->zf, &ze, &se);
			stream->eof = 1;
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Zip stream error: %s", zip_file_strerror(self->zf));
			return 0;
		}
		/* cast count to signed value to avoid possibly negative n
		 * being cast to unsigned value */
		if (n == 0 || n < (ssize_t)count) {
			stream->eof = 1;
		} else {
			self->cursor += n;
		}
	}
	return (n < 1 ? 0 : (size_t)n);
}
/* }}} */

/* {{{ jonj_zip_ops_write */
static size_t jonj_zip_ops_write(jonj_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	if (!stream) {
		return 0;
	}

	return count;
}
/* }}} */

/* {{{ jonj_zip_ops_close */
static int jonj_zip_ops_close(jonj_stream *stream, int close_handle TSRMLS_DC)
{
	STREAM_DATA_FROM_STREAM();
	if (close_handle) {
		if (self->zf) {
			zip_fclose(self->zf);
			self->zf = NULL;
		}

		if (self->za) {
			zip_close(self->za);
			self->za = NULL;
		}
	}
	efree(self);
	stream->abstract = NULL;
	return EOF;
}
/* }}} */

/* {{{ jonj_zip_ops_flush */
static int jonj_zip_ops_flush(jonj_stream *stream TSRMLS_DC)
{
	if (!stream) {
		return 0;
	}

	return 0;
}
/* }}} */

static int jonj_zip_ops_stat(jonj_stream *stream, jonj_stream_statbuf *ssb TSRMLS_DC) /* {{{ */
{
	struct zip_stat sb;
	const char *path = stream->orig_path;
	size_t path_len = strlen(stream->orig_path);
	char *file_basename;
	size_t file_basename_len;
	char file_dirname[MAXPATHLEN];
	struct zip *za;
	char *fragment;
	size_t fragment_len;
	int err;

	fragment = strchr(path, '#');
	if (!fragment) {
		return -1;
	}


	if (strncasecmp("zip://", path, 6) == 0) {
		path += 6;
	}

	fragment_len = strlen(fragment);

	if (fragment_len < 1) {
		return -1;
	}
	path_len = strlen(path);
	if (path_len >= MAXPATHLEN) {
		return -1;
	}

	memcpy(file_dirname, path, path_len - fragment_len);
	file_dirname[path_len - fragment_len] = '\0';

	jonj_basename((char *)path, path_len - fragment_len, NULL, 0, &file_basename, &file_basename_len TSRMLS_CC);
	fragment++;

	if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname)) {
		efree(file_basename);
		return -1;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		memset(ssb, 0, sizeof(jonj_stream_statbuf));
		if (zip_stat(za, fragment, ZIP_FL_NOCASE, &sb) != 0) {
			efree(file_basename);
			return -1;
		}
		zip_close(za);

		if (path[path_len-1] != '/') {
			ssb->sb.st_size = sb.size;
			ssb->sb.st_mode |= S_IFREG; /* regular file */
		} else {
			ssb->sb.st_size = 0;
			ssb->sb.st_mode |= S_IFDIR; /* regular directory */
		}

		ssb->sb.st_mtime = sb.mtime;
		ssb->sb.st_atime = sb.mtime;
		ssb->sb.st_ctime = sb.mtime;
		ssb->sb.st_nlink = 1;
		ssb->sb.st_rdev = -1;
#ifndef JONJ_WIN32
		ssb->sb.st_blksize = -1;
		ssb->sb.st_blocks = -1;
#endif
		ssb->sb.st_ino = -1;
	}
	efree(file_basename);
	return 0;
}
/* }}} */

jonj_stream_ops jonj_stream_zipio_ops = {
	jonj_zip_ops_write, jonj_zip_ops_read,
	jonj_zip_ops_close, jonj_zip_ops_flush,
	"zip",
	NULL, /* seek */
	NULL, /* cast */
	jonj_zip_ops_stat, /* stat */
	NULL  /* set_option */
};

/* {{{ jonj_stream_zip_open */
jonj_stream *jonj_stream_zip_open(const char *filename, const char *path, const char *mode STREAMS_DC TSRMLS_DC)
{
	struct zip_file *zf = NULL;
	int err = 0;

	jonj_stream *stream = NULL;
	struct jonj_zip_stream_data_t *self;
	struct zip *stream_za;

	if (strncmp(mode,"r", strlen("r")) != 0) {
		return NULL;
	}

	if (filename) {
		if (ZIP_OPENBASEDIR_CHECKPATH(filename)) {
			return NULL;
		}

		/* duplicate to make the stream za independent (esp. for MSHUTDOWN) */
		stream_za = zip_open(filename, ZIP_CREATE, &err);
		if (!stream_za) {
			return NULL;
		}

		zf = zip_fopen(stream_za, path, 0);
		if (zf) {
			self = emalloc(sizeof(*self));

			self->za = stream_za;
			self->zf = zf;
			self->stream = NULL;
			self->cursor = 0;
			stream = jonj_stream_alloc(&jonj_stream_zipio_ops, self, NULL, mode);
			stream->orig_path = estrdup(path);
		} else {
			zip_close(stream_za);
		}
	}

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}

}
/* }}} */

/* {{{ jonj_stream_zip_opener */
jonj_stream *jonj_stream_zip_opener(jonj_stream_wrapper *wrapper,
											const char *path,
											const char *mode,
											int options,
											char **opened_path,
											jonj_stream_context *context STREAMS_DC TSRMLS_DC)
{
	size_t path_len;

	char *file_basename;
	size_t file_basename_len;
	char file_dirname[MAXPATHLEN];

	struct zip *za;
	struct zip_file *zf = NULL;
	char *fragment;
	size_t fragment_len;
	int err;

	jonj_stream *stream = NULL;
	struct jonj_zip_stream_data_t *self;

	fragment = strchr(path, '#');
	if (!fragment) {
		return NULL;
	}

	if (strncasecmp("zip://", path, 6) == 0) {
		path += 6;
	}

	fragment_len = strlen(fragment);

	if (fragment_len < 1) {
		return NULL;
	}
	path_len = strlen(path);
	if (path_len >= MAXPATHLEN || mode[0] != 'r') {
		return NULL;
	}

	memcpy(file_dirname, path, path_len - fragment_len);
	file_dirname[path_len - fragment_len] = '\0';

	jonj_basename(path, path_len - fragment_len, NULL, 0, &file_basename, &file_basename_len TSRMLS_CC);
	fragment++;

	if (ZIP_OPENBASEDIR_CHECKPATH(file_dirname)) {
		efree(file_basename);
		return NULL;
	}

	za = zip_open(file_dirname, ZIP_CREATE, &err);
	if (za) {
		zf = zip_fopen(za, fragment, 0);
		if (zf) {
			self = emalloc(sizeof(*self));

			self->za = za;
			self->zf = zf;
			self->stream = NULL;
			self->cursor = 0;
			stream = jonj_stream_alloc(&jonj_stream_zipio_ops, self, NULL, mode);

			if (opened_path) {
				*opened_path = estrdup(path);
			}
		} else {
			zip_close(za);
		}
	}

	efree(file_basename);

	if (!stream) {
		return NULL;
	} else {
		return stream;
	}
}
/* }}} */

static jonj_stream_wrapper_ops zip_stream_wops = {
	jonj_stream_zip_opener,
	NULL,	/* close */
	NULL,	/* fstat */
	NULL,	/* stat */
	NULL,	/* opendir */
	"zip wrapper",
	NULL,	/* unlink */
	NULL,	/* rename */
	NULL,	/* mkdir */
	NULL	/* rmdir */
};

jonj_stream_wrapper jonj_stream_zip_wrapper = {
	&zip_stream_wops,
	NULL,
	0 /* is_url */
};
#endif /* ZEND_ENGINE_2 */
#endif /* HAVE_ZIP */
