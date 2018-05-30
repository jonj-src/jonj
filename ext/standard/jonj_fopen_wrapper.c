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
   | Authors: Rasmus Lerdorf <rasmus@jonj.tk>                             |
   |          Jim Winstead <jimw@jonj.tk>                                 |
   |          Hartmut Holzgraefe <hholzgra@jonj.tk>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "jonj.h"
#include "jonj_globals.h"
#include "jonj_standard.h"
#include "jonj_fopen_wrappers.h"
#include "SAPI.h"

static size_t jonj_stream_output_write(jonj_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	JONJWRITE(buf, count);
	return count;
}
/* }}} */

static size_t jonj_stream_output_read(jonj_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	stream->eof = 1;
	return 0;
}
/* }}} */

static int jonj_stream_output_close(jonj_stream *stream, int close_handle TSRMLS_DC) /* {{{ */
{
	return 0;
}
/* }}} */

jonj_stream_ops jonj_stream_output_ops = {
	jonj_stream_output_write,
	jonj_stream_output_read,
	jonj_stream_output_close,
	NULL, /* flush */
	"Output",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

typedef struct jonj_stream_input { /* {{{ */
	jonj_stream *body;
	off_t position;
} jonj_stream_input_t;
/* }}} */

static size_t jonj_stream_input_write(jonj_stream *stream, const char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	return -1;
}
/* }}} */

static size_t jonj_stream_input_read(jonj_stream *stream, char *buf, size_t count TSRMLS_DC) /* {{{ */
{
	jonj_stream_input_t *input = stream->abstract;
	size_t read;

	if (!SG(post_read) && SG(read_post_bytes) < input->position + count) {
		/* read requested data from SAPI */
		int read_bytes = sapi_read_post_block(buf, count TSRMLS_CC);

		if (read_bytes > 0) {
			jonj_stream_seek(input->body, 0, SEEK_END);
			jonj_stream_write(input->body, buf, read_bytes);
		}
	}

	if (!input->body->readfilters.head) {
		/* If the input stream contains filters, it's not really seekable. The
			input->position is likely to be wrong for unfiltered data. */
		jonj_stream_seek(input->body, input->position, SEEK_SET);
	}
	read = jonj_stream_read(input->body, buf, count);

	if (!read || read == (size_t) -1) {
		stream->eof = 1;
	} else {
		input->position += read;
	}

	return read;
}
/* }}} */

static int jonj_stream_input_close(jonj_stream *stream, int close_handle TSRMLS_DC) /* {{{ */
{
	efree(stream->abstract);
	stream->abstract = NULL;

	return 0;
}
/* }}} */

static int jonj_stream_input_flush(jonj_stream *stream TSRMLS_DC) /* {{{ */
{
	return -1;
}
/* }}} */

static int jonj_stream_input_seek(jonj_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC) /* {{{ */
{
	jonj_stream_input_t *input = stream->abstract;

	if (input->body) {
		int sought = jonj_stream_seek(input->body, offset, whence);
		*newoffset = (input->body)->position;
		return sought;
	}

	return -1;
}
/* }}} */

jonj_stream_ops jonj_stream_input_ops = {
	jonj_stream_input_write,
	jonj_stream_input_read,
	jonj_stream_input_close,
	jonj_stream_input_flush,
	"Input",
	jonj_stream_input_seek,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

static void jonj_stream_apply_filter_list(jonj_stream *stream, char *filterlist, int read_chain, int write_chain TSRMLS_DC) /* {{{ */
{
	char *p, *token;
	jonj_stream_filter *temp_filter;

	p = jonj_strtok_r(filterlist, "|", &token);
	while (p) {
		jonj_url_decode(p, strlen(p));
		if (read_chain) {
			if ((temp_filter = jonj_stream_filter_create(p, NULL, jonj_stream_is_persistent(stream) TSRMLS_CC))) {
				jonj_stream_filter_append(&stream->readfilters, temp_filter);
			} else {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create filter (%s)", p);
			}
		}
		if (write_chain) {
			if ((temp_filter = jonj_stream_filter_create(p, NULL, jonj_stream_is_persistent(stream) TSRMLS_CC))) {
				jonj_stream_filter_append(&stream->writefilters, temp_filter);
			} else {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create filter (%s)", p);
			}
		}
		p = jonj_strtok_r(NULL, "|", &token);
	}
}
/* }}} */

jonj_stream * jonj_stream_url_wrap_jonj(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options,
									 char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC) /* {{{ */
{
	int fd = -1;
	int mode_rw = 0;
	jonj_stream * stream = NULL;
	char *p, *token, *pathdup;
	long max_memory;
	FILE *file = NULL;

	if (!strncasecmp(path, "jonj://", 6)) {
		path += 6;
	}

	if (!strncasecmp(path, "temp", 4)) {
		path += 4;
		max_memory = JONJ_STREAM_MAX_MEM;
		if (!strncasecmp(path, "/maxmemory:", 11)) {
			path += 11;
			max_memory = strtol(path, NULL, 10);
			if (max_memory < 0) {
				jonj_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Max memory must be >= 0");
				return NULL;
			}
		}
		if (strpbrk(mode, "wa+")) {
			mode_rw = TEMP_STREAM_DEFAULT;
		} else {
			mode_rw = TEMP_STREAM_READONLY;
		}
		return jonj_stream_temp_create(mode_rw, max_memory);
	}

	if (!strcasecmp(path, "memory")) {
		if (strpbrk(mode, "wa+")) {
			mode_rw = TEMP_STREAM_DEFAULT;
		} else {
			mode_rw = TEMP_STREAM_READONLY;
		}
		return jonj_stream_memory_create(mode_rw);
	}

	if (!strcasecmp(path, "output")) {
		return jonj_stream_alloc(&jonj_stream_output_ops, NULL, 0, "wb");
	}

	if (!strcasecmp(path, "input")) {
		jonj_stream_input_t *input;

		if ((options & STREAM_OPEN_FOR_INCLUDE) && !PG(allow_url_include) ) {
			if (options & REPORT_ERRORS) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "URL file-access is disabled in the server configuration");
			}
			return NULL;
		}

		input = ecalloc(1, sizeof(*input));
		if ((input->body = SG(request_info).request_body)) {
			jonj_stream_rewind(input->body);
		} else {
			input->body = jonj_stream_temp_create_ex(TEMP_STREAM_DEFAULT, SAPI_POST_BLOCK_SIZE, PG(upload_tmp_dir));
			SG(request_info).request_body = input->body;
		}

		return jonj_stream_alloc(&jonj_stream_input_ops, input, 0, "rb");
	}

	if (!strcasecmp(path, "stdin")) {
		if ((options & STREAM_OPEN_FOR_INCLUDE) && !PG(allow_url_include) ) {
			if (options & REPORT_ERRORS) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "URL file-access is disabled in the server configuration");
			}
			return NULL;
		}
		if (!strcmp(sapi_module.name, "cli")) {
			static int cli_in = 0;
			fd = STDIN_FILENO;
			if (cli_in) {
				fd = dup(fd);
			} else {
				cli_in = 1;
				file = stdin;
			}
		} else {
			fd = dup(STDIN_FILENO);
		}
	} else if (!strcasecmp(path, "stdout")) {
		if (!strcmp(sapi_module.name, "cli")) {
			static int cli_out = 0;
			fd = STDOUT_FILENO;
			if (cli_out++) {
				fd = dup(fd);
			} else {
				cli_out = 1;
				file = stdout;
			}
		} else {
			fd = dup(STDOUT_FILENO);
		}
	} else if (!strcasecmp(path, "stderr")) {
		if (!strcmp(sapi_module.name, "cli")) {
			static int cli_err = 0;
			fd = STDERR_FILENO;
			if (cli_err++) {
				fd = dup(fd);
			} else {
				cli_err = 1;
				file = stderr;
			}
		} else {
			fd = dup(STDERR_FILENO);
		}
	} else if (!strncasecmp(path, "fd/", 3)) {
		const char *start;
		char       *end;
		long	   fildes_ori;
		int		   dtablesize;

		if (strcmp(sapi_module.name, "cli")) {
			if (options & REPORT_ERRORS) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Direct access to file descriptors is only available from command-line JONJ");
			}
			return NULL;
		}

		if ((options & STREAM_OPEN_FOR_INCLUDE) && !PG(allow_url_include) ) {
			if (options & REPORT_ERRORS) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "URL file-access is disabled in the server configuration");
			}
			return NULL;
		}

		start = &path[3];
		fildes_ori = strtol(start, &end, 10);
		if (end == start || *end != '\0') {
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC,
				"jonj://fd/ stream must be specified in the form jonj://fd/<orig fd>");
			return NULL;
		}

#if HAVE_UNISTD_H
		dtablesize = getdtablesize();
#else
		dtablesize = INT_MAX;
#endif

		if (fildes_ori < 0 || fildes_ori >= dtablesize) {
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC,
				"The file descriptors must be non-negative numbers smaller than %d", dtablesize);
			return NULL;
		}
		
		fd = dup(fildes_ori);
		if (fd == -1) {
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC,
				"Error duping file descriptor %ld; possibly it doesn't exist: "
				"[%d]: %s", fildes_ori, errno, strerror(errno));
			return NULL;
		}
	} else if (!strncasecmp(path, "filter/", 7)) {
		/* Save time/memory when chain isn't specified */
		if (strchr(mode, 'r') || strchr(mode, '+')) {
			mode_rw |= JONJ_STREAM_FILTER_READ;
		}
		if (strchr(mode, 'w') || strchr(mode, '+') || strchr(mode, 'a')) {
			mode_rw |= JONJ_STREAM_FILTER_WRITE;
		}
		pathdup = estrndup(path + 6, strlen(path + 6));
		p = strstr(pathdup, "/resource=");
		if (!p) {
			jonj_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "No URL resource specified");
			efree(pathdup);
			return NULL;
		}
		if (!(stream = jonj_stream_open_wrapper(p + 10, mode, options, opened_path))) {
			efree(pathdup);
			return NULL;
		}

		*p = '\0';

		p = jonj_strtok_r(pathdup + 1, "/", &token);
		while (p) {
			if (!strncasecmp(p, "read=", 5)) {
				jonj_stream_apply_filter_list(stream, p + 5, 1, 0 TSRMLS_CC);
			} else if (!strncasecmp(p, "write=", 6)) {
				jonj_stream_apply_filter_list(stream, p + 6, 0, 1 TSRMLS_CC);
			} else {
				jonj_stream_apply_filter_list(stream, p, mode_rw & JONJ_STREAM_FILTER_READ, mode_rw & JONJ_STREAM_FILTER_WRITE TSRMLS_CC);
			}
			p = jonj_strtok_r(NULL, "/", &token);
		}
		efree(pathdup);

		return stream;
	} else {
		/* invalid jonj://thingy */
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid jonj:// URL specified");
		return NULL;
	}

	/* must be stdin, stderr or stdout */
	if (fd == -1)	{
		/* failed to dup */
		return NULL;
	}

#if defined(S_IFSOCK) && !defined(WIN32) && !defined(__BEOS__)
	do {
		struct stat st;
		memset(&st, 0, sizeof(st));
		if (fstat(fd, &st) == 0 && (st.st_mode & S_IFMT) == S_IFSOCK) {
			stream = jonj_stream_sock_open_from_socket(fd, NULL);
			if (stream) {
				stream->ops = &jonj_stream_socket_ops;
				return stream;
			}
		}
	} while (0);
#endif

	if (file) {
		stream = jonj_stream_fopen_from_file(file, mode);
	} else {
		stream = jonj_stream_fopen_from_fd(fd, mode, NULL);
		if (stream == NULL) {
			close(fd);
		}
	}

	return stream;
}
/* }}} */

static jonj_stream_wrapper_ops jonj_stdio_wops = {
	jonj_stream_url_wrap_jonj,
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"JONJ",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL  /* rmdir */
};

jonj_stream_wrapper jonj_stream_jonj_wrapper =	{
	&jonj_stdio_wops,
	NULL,
	0, /* is_url */
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
