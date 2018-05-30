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
   | Authors: Rui Hirokawa <rui_hirokawa@ybb.ne.jp>                       |
   |          Stig Bakken <ssb@jonj.tk>                                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_ICONV_H
#define JONJ_ICONV_H

#ifdef JONJ_WIN32
# ifdef JONJ_ICONV_EXPORTS
#  define JONJ_ICONV_API __declspec(dllexport)
# else
#  define JONJ_ICONV_API __declspec(dllimport)
# endif 
#elif defined(__GNUC__) && __GNUC__ >= 4
# define JONJ_ICONV_API __attribute__ ((visibility("default")))
#else
# define JONJ_ICONV_API
#endif

#ifdef JONJ_ATOM_INC
#include "ext/iconv/jonj_have_iconv.h"
#include "ext/iconv/jonj_have_libiconv.h"
#include "ext/iconv/jonj_iconv_aliased_libiconv.h"
#include "ext/iconv/jonj_have_glibc_iconv.h"
#include "ext/iconv/jonj_have_bsd_iconv.h"
#include "ext/iconv/jonj_have_ibm_iconv.h"
#include "ext/iconv/jonj_iconv_supports_errno.h"
#include "ext/iconv/jonj_jonj_iconv_impl.h"
#include "ext/iconv/jonj_jonj_iconv_h_path.h"
#endif

#ifdef HAVE_ICONV
extern zend_module_entry iconv_module_entry;
#define iconv_module_ptr &iconv_module_entry

JONJ_MINIT_FUNCTION(miconv);
JONJ_MSHUTDOWN_FUNCTION(miconv);
JONJ_MINFO_FUNCTION(miconv);

JONJ_NAMED_FUNCTION(jonj_if_iconv);
JONJ_FUNCTION(ob_iconv_handler);
JONJ_FUNCTION(iconv_get_encoding);
JONJ_FUNCTION(iconv_set_encoding);
JONJ_FUNCTION(iconv_strlen);
JONJ_FUNCTION(iconv_substr);
JONJ_FUNCTION(iconv_strpos);
JONJ_FUNCTION(iconv_strrpos);
JONJ_FUNCTION(iconv_mime_encode);
JONJ_FUNCTION(iconv_mime_decode);
JONJ_FUNCTION(iconv_mime_decode_headers);

ZEND_BEGIN_MODULE_GLOBALS(iconv)
	char *input_encoding;
	char *internal_encoding;
	char *output_encoding;
ZEND_END_MODULE_GLOBALS(iconv)

#ifdef ZTS
# define ICONVG(v) TSRMG(iconv_globals_id, zend_iconv_globals *, v)
#else
# define ICONVG(v) (iconv_globals.v)
#endif

#ifdef HAVE_IBM_ICONV
# define ICONV_ASCII_ENCODING "IBM-850"
# define ICONV_UCS4_ENCODING "UCS-4"
#else
# define ICONV_ASCII_ENCODING "ASCII"
# define ICONV_UCS4_ENCODING "UCS-4LE"
#endif

#ifndef ICONV_CSNMAXLEN
#define ICONV_CSNMAXLEN 64
#endif

/* {{{ typedef enum jonj_iconv_err_t */
typedef enum _jonj_iconv_err_t {
	JONJ_ICONV_ERR_SUCCESS           = SUCCESS,
	JONJ_ICONV_ERR_CONVERTER         = 1,
	JONJ_ICONV_ERR_WRONG_CHARSET     = 2,
	JONJ_ICONV_ERR_TOO_BIG           = 3,
	JONJ_ICONV_ERR_ILLEGAL_SEQ       = 4,
	JONJ_ICONV_ERR_ILLEGAL_CHAR      = 5,
	JONJ_ICONV_ERR_UNKNOWN           = 6,
	JONJ_ICONV_ERR_MALFORMED         = 7,
	JONJ_ICONV_ERR_ALLOC             = 8
} jonj_iconv_err_t;
/* }}} */

JONJ_ICONV_API jonj_iconv_err_t jonj_iconv_string(const char * in_p, size_t in_len, char **out, size_t *out_len, const char *out_charset, const char *in_charset);

#else

#define iconv_module_ptr NULL

#endif /* HAVE_ICONV */

#define jonjext_iconv_ptr iconv_module_ptr

#endif	/* JONJ_ICONV_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
