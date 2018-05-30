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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stefan R�hrich <sr@linux.de>                                |
   |          Michael Wallner <mike@jonj.tk>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_ZLIB_H
#define JONJ_ZLIB_H

#include <zlib.h>

#define JONJ_ZLIB_ENCODING_RAW		-0xf
#define JONJ_ZLIB_ENCODING_GZIP		0x1f
#define JONJ_ZLIB_ENCODING_DEFLATE	0x0f

#define JONJ_ZLIB_ENCODING_ANY		0x2f

#define JONJ_ZLIB_OUTPUT_HANDLER_NAME "zlib output compression"
#define JONJ_ZLIB_BUFFER_SIZE_GUESS(in_len) (((size_t) ((double) in_len * (double) 1.015)) + 10 + 8 + 4 + 1)

typedef struct _jonj_zlib_buffer {
	char *data;
	char *aptr;
	size_t used;
	size_t free;
	size_t size;
} jonj_zlib_buffer;

typedef struct _jonj_zlib_context {
	z_stream Z;
	jonj_zlib_buffer buffer;
} jonj_zlib_context;

ZEND_BEGIN_MODULE_GLOBALS(zlib)
	/* variables for transparent gzip encoding */
	int compression_coding;
	long output_compression;
	long output_compression_level;
	char *output_handler;
	jonj_zlib_context *ob_gzhandler;
	long output_compression_default;
    zend_bool handler_registered;
ZEND_END_MODULE_GLOBALS(zlib);

jonj_stream *jonj_stream_gzopen(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC);
extern jonj_stream_ops jonj_stream_gzio_ops;
extern jonj_stream_wrapper jonj_stream_gzip_wrapper;
extern jonj_stream_filter_factory jonj_zlib_filter_factory;
extern zend_module_entry jonj_zlib_module_entry;
#define zlib_module_ptr &jonj_zlib_module_entry
#define jonjext_zlib_ptr zlib_module_ptr

#ifdef ZTS
# include "TSRM.h"
# define ZLIBG(v) TSRMG(zlib_globals_id, zend_zlib_globals *, v)
#else
# define ZLIBG(v) (zlib_globals.v)
#endif

#endif /* JONJ_ZLIB_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
