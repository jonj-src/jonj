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
   | Author: Marcus Boerger <helly@jonj.tk>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_MEMORY_STREAM_H
#define JONJ_MEMORY_STREAM_H

#include "jonj_streams.h"

#define JONJ_STREAM_MAX_MEM	2 * 1024 * 1024

#define TEMP_STREAM_DEFAULT  0
#define TEMP_STREAM_READONLY 1
#define TEMP_STREAM_TAKE_BUFFER 2

#define jonj_stream_memory_create(mode) _jonj_stream_memory_create((mode) STREAMS_CC TSRMLS_CC)
#define jonj_stream_memory_create_rel(mode) _jonj_stream_memory_create((mode) STREAMS_REL_CC TSRMLS_CC)
#define jonj_stream_memory_open(mode, buf, length) _jonj_stream_memory_open((mode), (buf), (length) STREAMS_CC TSRMLS_CC)
#define jonj_stream_memory_get_buffer(stream, length) _jonj_stream_memory_get_buffer((stream), (length) STREAMS_CC TSRMLS_CC)

#define jonj_stream_temp_new() jonj_stream_temp_create(TEMP_STREAM_DEFAULT, JONJ_STREAM_MAX_MEM)
#define jonj_stream_temp_create(mode, max_memory_usage) _jonj_stream_temp_create((mode), (max_memory_usage) STREAMS_CC TSRMLS_CC)
#define jonj_stream_temp_create_ex(mode, max_memory_usage, tmpdir) _jonj_stream_temp_create_ex((mode), (max_memory_usage), (tmpdir) STREAMS_CC TSRMLS_CC)
#define jonj_stream_temp_create_rel(mode, max_memory_usage) _jonj_stream_temp_create((mode), (max_memory_usage) STREAMS_REL_CC TSRMLS_CC)
#define jonj_stream_temp_open(mode, max_memory_usage, buf, length) _jonj_stream_temp_open((mode), (max_memory_usage), (buf), (length) STREAMS_CC TSRMLS_CC)

BEGIN_EXTERN_C()
JONJAPI jonj_stream *_jonj_stream_memory_create(int mode STREAMS_DC TSRMLS_DC);
JONJAPI jonj_stream *_jonj_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC TSRMLS_DC);
JONJAPI char *_jonj_stream_memory_get_buffer(jonj_stream *stream, size_t *length STREAMS_DC TSRMLS_DC);

JONJAPI jonj_stream *_jonj_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC TSRMLS_DC);
JONJAPI jonj_stream *_jonj_stream_temp_create_ex(int mode, size_t max_memory_usage, const char *tmpdir STREAMS_DC TSRMLS_DC);
JONJAPI jonj_stream *_jonj_stream_temp_open(int mode, size_t max_memory_usage, char *buf, size_t length STREAMS_DC TSRMLS_DC);
END_EXTERN_C()

extern JONJAPI jonj_stream_ops jonj_stream_memory_ops;
extern JONJAPI jonj_stream_ops jonj_stream_temp_ops;
extern JONJAPI jonj_stream_ops jonj_stream_rfc2397_ops;
extern JONJAPI jonj_stream_wrapper jonj_stream_rfc2397_wrapper;

#define JONJ_STREAM_IS_MEMORY &jonj_stream_memory_ops
#define JONJ_STREAM_IS_TEMP   &jonj_stream_temp_ops

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
