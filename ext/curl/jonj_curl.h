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
   | Author: Sterling Hughes <sterling@jonj.tk>                           |
   |         Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef _JONJ_CURL_H
#define _JONJ_CURL_H

#include "jonj.h"
#include "ext/standard/jonj_smart_str.h"

#ifdef COMPILE_DL_CURL
#undef HAVE_CURL
#define HAVE_CURL 1
#endif

#if HAVE_CURL

#define JONJ_CURL_DEBUG 0

#ifdef JONJ_WIN32
# define JONJ_CURL_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define JONJ_CURL_API __attribute__ ((visibility("default")))
#else
# define JONJ_CURL_API
#endif

#include <curl/curl.h>
#include <curl/multi.h>

extern zend_module_entry curl_module_entry;
#define curl_module_ptr &curl_module_entry

#define CURLOPT_RETURNTRANSFER 19913
#define CURLOPT_BINARYTRANSFER 19914 /* For Backward compatibility */
#define JONJ_CURL_STDOUT 0
#define JONJ_CURL_FILE   1
#define JONJ_CURL_USER   2
#define JONJ_CURL_DIRECT 3
#define JONJ_CURL_RETURN 4
#define JONJ_CURL_IGNORE 7

extern int  le_curl;
#define le_curl_name "cURL handle"
extern int  le_curl_multi_handle;
#define le_curl_multi_handle_name "cURL Multi Handle"
extern int  le_curl_share_handle;
#define le_curl_share_handle_name "cURL Share Handle"

JONJ_MINIT_FUNCTION(curl);
JONJ_MSHUTDOWN_FUNCTION(curl);
JONJ_MINFO_FUNCTION(curl);

JONJ_FUNCTION(curl_close);
JONJ_FUNCTION(curl_copy_handle);
JONJ_FUNCTION(curl_errno);
JONJ_FUNCTION(curl_error);
JONJ_FUNCTION(curl_exec);
JONJ_FUNCTION(curl_getinfo);
JONJ_FUNCTION(curl_init);
JONJ_FUNCTION(curl_setopt);
JONJ_FUNCTION(curl_setopt_array);
JONJ_FUNCTION(curl_version);

JONJ_FUNCTION(curl_multi_add_handle);
JONJ_FUNCTION(curl_multi_close);
JONJ_FUNCTION(curl_multi_exec);
JONJ_FUNCTION(curl_multi_getcontent);
JONJ_FUNCTION(curl_multi_info_read);
JONJ_FUNCTION(curl_multi_init);
JONJ_FUNCTION(curl_multi_remove_handle);
JONJ_FUNCTION(curl_multi_select);

JONJ_FUNCTION(curl_share_close);
JONJ_FUNCTION(curl_share_init);
JONJ_FUNCTION(curl_share_setopt);

#if LIBCURL_VERSION_NUM >= 0x070c00 /* 7.12.0 */
JONJ_FUNCTION(curl_strerror);
JONJ_FUNCTION(curl_multi_strerror);
#endif

#if LIBCURL_VERSION_NUM >= 0x070c01 /* 7.12.1 */
JONJ_FUNCTION(curl_reset);
#endif

#if LIBCURL_VERSION_NUM >= 0x070f04 /* 7.15.4 */
JONJ_FUNCTION(curl_escape);
JONJ_FUNCTION(curl_unescape);

JONJ_FUNCTION(curl_multi_setopt);
#endif

#if LIBCURL_VERSION_NUM >= 0x071200 /* 7.18.0 */
JONJ_FUNCTION(curl_pause);
#endif
JONJ_FUNCTION(curl_file_create);


void _jonj_curl_multi_close(zend_rsrc_list_entry * TSRMLS_DC);
void _jonj_curl_share_close(zend_rsrc_list_entry * TSRMLS_DC);

typedef struct {
	zval            *func_name;
	zend_fcall_info_cache fci_cache;
	FILE            *fp;
	smart_str       buf;
	int             method;
	zval		*stream;
} jonj_curl_write;

typedef struct {
	zval            *func_name;
	zend_fcall_info_cache fci_cache;
	FILE            *fp;
	long            fd;
	int             method;
	zval		*stream;
} jonj_curl_read;

typedef struct {
	zval 		*func_name;
	zend_fcall_info_cache fci_cache;
	int    	        method;
} jonj_curl_progress, jonj_curl_fnmatch;

typedef struct {
	jonj_curl_write *write;
	jonj_curl_write *write_header;
	jonj_curl_read  *read;
#if CURLOPT_PASSWDFUNCTION != 0
	zval           *passwd;
#endif
	zval           *std_err;
	jonj_curl_progress *progress;
#if LIBCURL_VERSION_NUM >= 0x071500 /* Available since 7.21.0 */
	jonj_curl_fnmatch  *fnmatch;
#endif
} jonj_curl_handlers;

struct _jonj_curl_error  {
	char str[CURL_ERROR_SIZE + 1];
	int  no;
};

struct _jonj_curl_send_headers {
	char *str;
	size_t str_len;
};

struct _jonj_curl_free {
	zend_llist str;
	zend_llist post;
	HashTable *slist;
};

typedef struct {
	struct _jonj_curl_error   err;
	struct _jonj_curl_free    *to_free;
	struct _jonj_curl_send_headers header;
	void ***thread_ctx;
	CURL                    *cp;
	jonj_curl_handlers       *handlers;
	long                     id;
	zend_bool                in_callback;
	zval                     *clone;
	zend_bool                safe_upload;
} jonj_curl;

#define CURLOPT_SAFE_UPLOAD -1

typedef struct {
	int    still_running;
	CURLM *multi;
	zend_llist easyh;
} jonj_curlm;

typedef struct {
	CURLSH                   *share;
} jonj_curlsh;

void _jonj_curl_cleanup_handle(jonj_curl *);
void _jonj_curl_multi_cleanup_list(void *data);
void _jonj_curl_verify_handlers(jonj_curl *ch, int reporterror TSRMLS_DC);

void curlfile_register_class(TSRMLS_D);
JONJ_CURL_API extern zend_class_entry *curl_CURLFile_class;

#else
#define curl_module_ptr NULL
#endif /* HAVE_CURL */
#define jonjext_curl_ptr curl_module_ptr
#endif  /* _JONJ_CURL_H */
