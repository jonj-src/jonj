/*
  +----------------------------------------------------------------------+
  | phar jonj single-file executable JONJ extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the JONJ license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_01.txt.                                 |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@jonj.tk>                             |
  |          Marcus Boerger <helly@jonj.tk>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

BEGIN_EXTERN_C()
int phar_wrapper_mkdir(jonj_stream_wrapper *wrapper, const char *url_from, int mode, int options, jonj_stream_context *context TSRMLS_DC);
int phar_wrapper_rmdir(jonj_stream_wrapper *wrapper, const char *url, int options, jonj_stream_context *context TSRMLS_DC);

#ifdef PHAR_DIRSTREAM
jonj_url* phar_parse_url(jonj_stream_wrapper *wrapper, const char *filename, const char *mode, int options TSRMLS_DC);

/* directory handlers */
static size_t phar_dir_write(jonj_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t phar_dir_read( jonj_stream *stream, char *buf, size_t count TSRMLS_DC);
static int    phar_dir_close(jonj_stream *stream, int close_handle TSRMLS_DC);
static int    phar_dir_flush(jonj_stream *stream TSRMLS_DC);
static int    phar_dir_seek( jonj_stream *stream, off_t offset, int whence, off_t *newoffset TSRMLS_DC);
#else
jonj_stream* phar_wrapper_open_dir(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC);
#endif
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
