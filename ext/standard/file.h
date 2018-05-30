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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with jonj 3.0 revision 1.30 1999-06-16 [ssb] */

#ifndef FILE_H
#define FILE_H

JONJ_MINIT_FUNCTION(file);
JONJ_MSHUTDOWN_FUNCTION(file);

JONJ_FUNCTION(tempnam);
JONJ_NAMED_FUNCTION(jonj_if_tmpfile);
JONJ_NAMED_FUNCTION(jonj_if_fopen);
JONJAPI JONJ_FUNCTION(fclose);
JONJ_FUNCTION(popen);
JONJ_FUNCTION(pclose);
JONJAPI JONJ_FUNCTION(feof);
JONJAPI JONJ_FUNCTION(fread);
JONJAPI JONJ_FUNCTION(fgetc);
JONJAPI JONJ_FUNCTION(fgets);
JONJ_FUNCTION(fscanf);
JONJAPI JONJ_FUNCTION(fgetss);
JONJ_FUNCTION(fgetcsv);
JONJ_FUNCTION(fputcsv);
JONJAPI JONJ_FUNCTION(fwrite);
JONJAPI JONJ_FUNCTION(fflush);
JONJAPI JONJ_FUNCTION(rewind);
JONJAPI JONJ_FUNCTION(ftell);
JONJAPI JONJ_FUNCTION(fseek);
JONJ_FUNCTION(mkdir);
JONJ_FUNCTION(rmdir);
JONJAPI JONJ_FUNCTION(fpassthru);
JONJ_FUNCTION(readfile);
JONJ_FUNCTION(umask);
JONJ_FUNCTION(rename);
JONJ_FUNCTION(unlink);
JONJ_FUNCTION(copy);
JONJ_FUNCTION(file);
JONJ_FUNCTION(file_get_contents);
JONJ_FUNCTION(file_put_contents);
JONJ_FUNCTION(get_meta_tags);
JONJ_FUNCTION(flock);
JONJ_FUNCTION(fd_set);
JONJ_FUNCTION(fd_isset);
#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
JONJ_FUNCTION(realpath);
#endif
#ifdef HAVE_FNMATCH
JONJ_FUNCTION(fnmatch);
#endif
JONJ_NAMED_FUNCTION(jonj_if_ftruncate);
JONJ_NAMED_FUNCTION(jonj_if_fstat);
JONJ_FUNCTION(sys_get_temp_dir);

JONJ_MINIT_FUNCTION(user_streams);

JONJAPI int jonj_le_stream_context(TSRMLS_D);
JONJAPI int jonj_set_sock_blocking(int socketd, int block TSRMLS_DC);
JONJAPI int jonj_copy_file(const char *src, const char *dest TSRMLS_DC);
JONJAPI int jonj_copy_file_ex(const char *src, const char *dest, int src_chk TSRMLS_DC);
JONJAPI int jonj_copy_file_ctx(const char *src, const char *dest, int src_chk, jonj_stream_context *ctx TSRMLS_DC);
JONJAPI int jonj_mkdir_ex(const char *dir, long mode, int options TSRMLS_DC);
JONJAPI int jonj_mkdir(const char *dir, long mode TSRMLS_DC);
JONJAPI void jonj_fgetcsv(jonj_stream *stream, char delimiter, char enclosure, char escape_char, size_t buf_len, char *buf, zval *return_value TSRMLS_DC);
JONJAPI int jonj_fputcsv(jonj_stream *stream, zval *fields, char delimiter, char enclosure, char escape_char TSRMLS_DC);

#define META_DEF_BUFSIZE 8192

#define JONJ_FILE_USE_INCLUDE_PATH 1
#define JONJ_FILE_IGNORE_NEW_LINES 2
#define JONJ_FILE_SKIP_EMPTY_LINES 4
#define JONJ_FILE_APPEND 8
#define JONJ_FILE_NO_DEFAULT_CONTEXT 16

typedef enum _jonj_meta_tags_token {
	TOK_EOF = 0,
	TOK_OPENTAG,
	TOK_CLOSETAG,
	TOK_SLASH,
	TOK_EQUAL,
	TOK_SPACE,
	TOK_ID,
	TOK_STRING,
	TOK_OTHER
} jonj_meta_tags_token;

typedef struct _jonj_meta_tags_data {
	jonj_stream *stream;
	int ulc;
	int lc;
	char *input_buffer;
	char *token_data;
	int token_len;
	int in_meta;
} jonj_meta_tags_data;

jonj_meta_tags_token jonj_next_meta_token(jonj_meta_tags_data * TSRMLS_DC);

typedef struct {
	int pclose_ret;
	size_t def_chunk_size;
	long auto_detect_line_endings;
	long default_socket_timeout;
	char *user_agent; /* for the http wrapper */
	char *from_address; /* for the ftp and http wrappers */
	const char *user_stream_current_filename; /* for simple recursion protection */
	jonj_stream_context *default_context;
	HashTable *stream_wrappers;			/* per-request copy of url_stream_wrappers_hash */
	HashTable *stream_filters;			/* per-request copy of stream_filters_hash */
	HashTable *wrapper_errors;			/* key: wrapper address; value: linked list of char* */
	int pclose_wait;
} jonj_file_globals;

#ifdef ZTS
#define FG(v) TSRMG(file_globals_id, jonj_file_globals *, v)
extern JONJAPI int file_globals_id;
#else
#define FG(v) (file_globals.v)
extern JONJAPI jonj_file_globals file_globals;
#endif


#endif /* FILE_H */

