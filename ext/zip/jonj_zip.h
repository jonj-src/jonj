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
  | Author: Pierre-Alain Joye <pajoye@jonj.tk>                           |
  +----------------------------------------------------------------------+
*/


#ifndef JONJ_ZIP_H
#define JONJ_ZIP_H

extern zend_module_entry zip_module_entry;
#define jonjext_zip_ptr &zip_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

#if defined(HAVE_LIBZIP)
#include <zip.h>
#else
#include "lib/zip.h"
#endif

#ifndef ZIP_OVERWRITE
#define ZIP_OVERWRITE ZIP_TRUNCATE
#endif

#define JONJ_ZIP_VERSION "1.12.5"

#if ((JONJ_MAJOR_VERSION >= 5 && JONJ_MINOR_VERSION >= 2) || JONJ_MAJOR_VERSION >= 6)
# define JONJ_ZIP_USE_OO 1
#endif

#ifndef  Z_SET_REFCOUNT_P
# if JONJ_MAJOR_VERSION < 6 && (JONJ_MAJOR_VERSION == 5 && JONJ_MINOR_VERSION < 3)
#  define Z_SET_REFCOUNT_P(pz, rc)  pz->refcount = rc 
#  define Z_UNSET_ISREF_P(pz) pz->is_ref = 0 
# endif
#endif

/* {{{ ZIP_OPENBASEDIR_CHECKPATH(filename) */
#if JONJ_API_VERSION < 20100412
# define ZIP_OPENBASEDIR_CHECKPATH(filename) \
	(PG(safe_mode) && (!jonj_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || jonj_check_open_basedir(filename TSRMLS_CC)
#else
#define ZIP_OPENBASEDIR_CHECKPATH(filename) \
	jonj_check_open_basedir(filename TSRMLS_CC)
#endif
/* }}} */

typedef struct _ze_zip_rsrc {
	struct zip *za;
	int index_current;
	int num_files;
} zip_rsrc;

typedef zip_rsrc * zip_rsrc_ptr;

typedef struct _ze_zip_read_rsrc {
	struct zip_file *zf;
	struct zip_stat sb;
} zip_read_rsrc;

#ifdef JONJ_ZIP_USE_OO 
#define ZIPARCHIVE_ME(name, arg_info, flags) {#name, c_ziparchive_ ##name, arg_info,(zend_uint) (sizeof(arg_info)/sizeof(struct _zend_arg_info)-1), flags },
#define ZIPARCHIVE_METHOD(name)	ZEND_NAMED_FUNCTION(c_ziparchive_ ##name)


/* Extends zend object */
typedef struct _ze_zip_object {
	zend_object zo;
	struct zip *za;
	int buffers_cnt;
	char **buffers;
	HashTable *prop_handler;
	char *filename;
	int filename_len;
} ze_zip_object;

jonj_stream *jonj_stream_zip_opener(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC);
jonj_stream *jonj_stream_zip_open(const char *filename, const char *path, const char *mode STREAMS_DC TSRMLS_DC);

extern jonj_stream_wrapper jonj_stream_zip_wrapper;
#endif

#endif	/* JONJ_ZIP_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
