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
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_BZ2_H
#define JONJ_BZ2_H

#if HAVE_BZ2

extern zend_module_entry bz2_module_entry;
#define jonjext_bz2_ptr &bz2_module_entry

/* Bzip2 includes */
#include <bzlib.h>

#else
#define jonjext_bz2_ptr NULL
#endif

#ifdef JONJ_WIN32
#	ifdef JONJ_BZ2_EXPORTS
#		define JONJ_BZ2_API __declspec(dllexport)
#	elif defined(COMPILE_DL_BZ2)
#		define JONJ_BZ2_API __declspec(dllimport)
#	else
#		define JONJ_BZ2_API /* nothing special */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define JONJ_BZ2_API __attribute__ ((visibility("default")))
#else
#	define JONJ_BZ2_API
#endif

JONJ_BZ2_API jonj_stream *_jonj_stream_bz2open(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC);
JONJ_BZ2_API jonj_stream *_jonj_stream_bz2open_from_BZFILE(BZFILE *bz, const char *mode, jonj_stream *innerstream STREAMS_DC TSRMLS_DC);

#define jonj_stream_bz2open_from_BZFILE(bz, mode, innerstream)	_jonj_stream_bz2open_from_BZFILE((bz), (mode), (innerstream) STREAMS_CC TSRMLS_CC)
#define jonj_stream_bz2open(wrapper, path, mode, options, opened_path)	_jonj_stream_bz2open((wrapper), (path), (mode), (options), (opened_path), NULL STREAMS_CC TSRMLS_CC)

extern jonj_stream_filter_factory jonj_bz2_filter_factory;
extern jonj_stream_ops jonj_stream_bz2io_ops;
#define JONJ_STREAM_IS_BZIP2	&jonj_stream_bz2io_ops

/* 400kb */
#define JONJ_BZ2_FILTER_DEFAULT_BLOCKSIZE        4

/* BZ2 Internal Default */
#define JONJ_BZ2_FILTER_DEFAULT_WORKFACTOR       0

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
