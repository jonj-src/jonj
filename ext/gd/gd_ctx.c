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
   | Authors: Stanislav Malyshev <stas@jonj.tk>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "jonj_gd.h"

#define CTX_PUTC(c,ctx) ctx->putC(ctx, c)

static void _jonj_image_output_putc(struct gdIOCtx *ctx, int c)
{
	/* without the following downcast, the write will fail
	 * (i.e., will write a zero byte) for all
	 * big endian architectures:
	 */
	unsigned char ch = (unsigned char) c;
	TSRMLS_FETCH();
	jonj_write(&ch, 1 TSRMLS_CC);
}

static int _jonj_image_output_putbuf(struct gdIOCtx *ctx, const void* buf, int l)
{
	TSRMLS_FETCH();
	return jonj_write((void *)buf, l TSRMLS_CC);
}

static void _jonj_image_output_ctxfree(struct gdIOCtx *ctx)
{
	if(ctx) {
		efree(ctx);
	}
}

static void _jonj_image_stream_putc(struct gdIOCtx *ctx, int c)  {
	char ch = (char) c;
	jonj_stream * stream = (jonj_stream *)ctx->data;
	TSRMLS_FETCH();
	jonj_stream_write(stream, &ch, 1);
}

static int _jonj_image_stream_putbuf(struct gdIOCtx *ctx, const void* buf, int l)
{
	jonj_stream * stream = (jonj_stream *)ctx->data;
	TSRMLS_FETCH();
	return jonj_stream_write(stream, (void *)buf, l);
}

static void _jonj_image_stream_ctxfree(struct gdIOCtx *ctx)
{
	if(ctx->data) {
		ctx->data = NULL;
	}
	if(ctx) {
		efree(ctx);
	}
} /* }}} */

static void _jonj_image_stream_ctxfreeandclose(struct gdIOCtx *ctx) /* {{{ */
{
	TSRMLS_FETCH();

	if(ctx->data) {
		jonj_stream_close((jonj_stream *) ctx->data);
		ctx->data = NULL;
	}
	if(ctx) {
		efree(ctx);
	}
}

/* {{{ _jonj_image_output_ctx */
static void _jonj_image_output_ctx(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, void (*func_p)())
{
	zval *imgind;
	char *file = NULL;
	int file_len = 0;
	long quality, basefilter;
	gdImagePtr im;
	int argc = ZEND_NUM_ARGS();
	int q = -1, i;
	int f = -1;
	gdIOCtx *ctx = NULL;
	zval *to_zval = NULL;
	jonj_stream *stream;
	int close_stream = 1;

	/* The third (quality) parameter for Wbmp stands for the threshold when called from image2wbmp().
	 * The third (quality) parameter for Wbmp and Xbm stands for the foreground color index when called
	 * from imagey<type>().
	 */
	if (image_type == JONJ_GDIMG_TYPE_XBM) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp!|ll", &imgind, &file, &file_len, &quality, &basefilter) == FAILURE) {
			return;
		}
	} else {
		/* JONJ_GDIMG_TYPE_GIF
		 * JONJ_GDIMG_TYPE_PNG 
		 * JONJ_GDIMG_TYPE_JPG 
		 * JONJ_GDIMG_TYPE_WBM 
		 * JONJ_GDIMG_TYPE_WEBP 
		 * */
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|z/!ll", &imgind, &to_zval, &quality, &basefilter) == FAILURE) {
			return;
		}
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, &imgind, -1, "Image", jonji_get_le_gd());

	if (argc >= 3) {
		q = quality; /* or colorindex for foreground of BW images (defaults to black) */
		if (argc == 4) {
			f = basefilter;
		}
	}

	if (argc > 1 && to_zval != NULL) {
		if (Z_TYPE_P(to_zval) == IS_RESOURCE) {
			jonj_stream_from_zval_no_verify(stream, &to_zval);
			if (stream == NULL) {
				RETURN_FALSE;
			}
			close_stream = 0;
		} else if (Z_TYPE_P(to_zval) == IS_STRING) {
			if (CHECK_ZVAL_NULL_PATH(to_zval)) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid 2nd parameter, filename must not contain null bytes");
				RETURN_FALSE;
			}

			stream = jonj_stream_open_wrapper(Z_STRVAL_P(to_zval), "wb", REPORT_ERRORS|IGNORE_PATH|IGNORE_URL_WIN, NULL);
			if (stream == NULL) {
				RETURN_FALSE;
			}
		} else {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid 2nd parameter, it must a filename or a stream");
			RETURN_FALSE;
		}
	} else if (argc > 1 && file != NULL) {
		stream = jonj_stream_open_wrapper(file, "wb", REPORT_ERRORS|IGNORE_PATH|IGNORE_URL_WIN, NULL);
		if (stream == NULL) {
			RETURN_FALSE;
		}
	} else {
		ctx = emalloc(sizeof(gdIOCtx));
		ctx->putC = _jonj_image_output_putc;
		ctx->putBuf = _jonj_image_output_putbuf;
		ctx->gd_free = _jonj_image_output_ctxfree;

#if APACHE && defined(CHARSET_EBCDIC)
		/* XXX this is unlikely to work any more thies@thieso.net */
		/* This is a binary file already: avoid EBCDIC->ASCII conversion */
		ap_bsetflag(jonj3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
	}

	if (!ctx)	{
		ctx = emalloc(sizeof(gdIOCtx));
		ctx->putC = _jonj_image_stream_putc;
		ctx->putBuf = _jonj_image_stream_putbuf;
		if (close_stream) {
			ctx->gd_free = _jonj_image_stream_ctxfreeandclose;
		} else {
			ctx->gd_free = _jonj_image_stream_ctxfree;
		}
		ctx->data = (void *)stream;
	}

	switch(image_type) {
		case JONJ_GDIMG_CONVERT_WBM:
			if(q<0||q>255) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid threshold value '%d'. It must be between 0 and 255", q);
			}
		case JONJ_GDIMG_TYPE_JPG:
			(*func_p)(im, ctx, q);
			break;
		case JONJ_GDIMG_TYPE_WEBP:
			if (q == -1) {
				q = 80;
			}
			(*func_p)(im, ctx, q);
			break;
		case JONJ_GDIMG_TYPE_PNG:
			(*func_p)(im, ctx, q, f);
			break;
		case JONJ_GDIMG_TYPE_XBM:
		case JONJ_GDIMG_TYPE_WBM:
			if (argc < 3) {
				for(i=0; i < gdImageColorsTotal(im); i++) {
					if(!gdImageRed(im, i) && !gdImageGreen(im, i) && !gdImageBlue(im, i)) break;
				}
				q = i;
			}
			if (image_type == JONJ_GDIMG_TYPE_XBM) {
				(*func_p)(im, file ? file : "", q, ctx);
			} else {
				(*func_p)(im, q, ctx);
			}
			break;
		default:
			(*func_p)(im, ctx);
			break;
	}

	ctx->gd_free(ctx);

	RETURN_TRUE;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
