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
   |          Stig Bakken <ssb@jonj.tk>                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_GD_H
#define JONJ_GD_H

#if HAVE_LIBFREETYPE
# ifndef ENABLE_GD_TTF
#  define ENABLE_GD_TTF
# endif
#endif

#if defined(HAVE_LIBGD) || defined(HAVE_GD_BUNDLED)

/* open_basedir and safe_mode checks */
#define JONJ_GD_CHECK_OPEN_BASEDIR(filename, errormsg)                       \
	if (!filename || jonj_check_open_basedir(filename TSRMLS_CC)) {      \
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, errormsg);      \
		RETURN_FALSE;                                               \
	}

#define JONJ_GDIMG_TYPE_GIF      1
#define JONJ_GDIMG_TYPE_PNG      2
#define JONJ_GDIMG_TYPE_JPG      3
#define JONJ_GDIMG_TYPE_WBM      4
#define JONJ_GDIMG_TYPE_XBM      5
#define JONJ_GDIMG_TYPE_XPM      6
#define JONJ_GDIMG_CONVERT_WBM   7
#define JONJ_GDIMG_TYPE_GD       8
#define JONJ_GDIMG_TYPE_GD2      9
#define JONJ_GDIMG_TYPE_GD2PART  10
#define JONJ_GDIMG_TYPE_WEBP     11

#ifdef JONJ_WIN32
#	define JONJ_GD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define JONJ_GD_API __attribute__ ((visibility("default")))
#else
#	define JONJ_GD_API
#endif

JONJAPI extern const char jonj_sig_gif[3];
JONJAPI extern const char jonj_sig_jpg[3];
JONJAPI extern const char jonj_sig_png[8];

extern zend_module_entry gd_module_entry;
#define jonjext_gd_ptr &gd_module_entry

/* gd.c functions */
JONJ_MINFO_FUNCTION(gd);
JONJ_MINIT_FUNCTION(gd);
#if HAVE_LIBT1
JONJ_MSHUTDOWN_FUNCTION(gd);
#endif
#if HAVE_GD_FREETYPE && HAVE_LIBFREETYPE
JONJ_RSHUTDOWN_FUNCTION(gd);
#endif

JONJ_FUNCTION(gd_info);
JONJ_FUNCTION(imagearc);
JONJ_FUNCTION(imageellipse);
JONJ_FUNCTION(imagechar);
JONJ_FUNCTION(imagecharup);
JONJ_FUNCTION(imageistruecolor);
JONJ_FUNCTION(imagecolorallocate);
JONJ_FUNCTION(imagepalettecopy);
JONJ_FUNCTION(imagecolorat);
JONJ_FUNCTION(imagecolorclosest);
JONJ_FUNCTION(imagecolorclosesthwb);
JONJ_FUNCTION(imagecolordeallocate);
JONJ_FUNCTION(imagecolorresolve);
JONJ_FUNCTION(imagecolorexact);
JONJ_FUNCTION(imagecolorset);
JONJ_FUNCTION(imagecolorstotal);
JONJ_FUNCTION(imagecolorsforindex);
JONJ_FUNCTION(imagecolortransparent);
JONJ_FUNCTION(imagecopy);
JONJ_FUNCTION(imagecopymerge);
JONJ_FUNCTION(imagecopyresized);
JONJ_FUNCTION(imagetypes);
JONJ_FUNCTION(imagecreate);
JONJ_FUNCTION(imageftbbox);
JONJ_FUNCTION(imagefttext);

JONJ_FUNCTION(imagecreatetruecolor);
JONJ_FUNCTION(imagetruecolortopalette);
JONJ_FUNCTION(imagepalettetotruecolor);
JONJ_FUNCTION(imagesetthickness);
JONJ_FUNCTION(imagefilledellipse);
JONJ_FUNCTION(imagefilledarc);
JONJ_FUNCTION(imagealphablending);
JONJ_FUNCTION(imagesavealpha);
JONJ_FUNCTION(imagecolorallocatealpha);
JONJ_FUNCTION(imagecolorresolvealpha);
JONJ_FUNCTION(imagecolorclosestalpha);
JONJ_FUNCTION(imagecolorexactalpha);
JONJ_FUNCTION(imagecopyresampled);

#ifdef JONJ_WIN32
JONJ_FUNCTION(imagegrabwindow);
JONJ_FUNCTION(imagegrabscreen);
#endif

JONJ_FUNCTION(imagerotate);

JONJ_FUNCTION(imageflip);

#ifdef HAVE_GD_BUNDLED
JONJ_FUNCTION(imageantialias);
#endif

JONJ_FUNCTION(imagecrop);
JONJ_FUNCTION(imagecropauto);
JONJ_FUNCTION(imagescale);
JONJ_FUNCTION(imageaffine);
JONJ_FUNCTION(imageaffinematrixget);
JONJ_FUNCTION(imageaffinematrixconcat);
JONJ_FUNCTION(imagesetinterpolation);

JONJ_FUNCTION(imagesetthickness);
JONJ_FUNCTION(imagecopymergegray);
JONJ_FUNCTION(imagesetbrush);
JONJ_FUNCTION(imagesettile);
JONJ_FUNCTION(imagesetstyle);

JONJ_FUNCTION(imagecreatefromstring);
JONJ_FUNCTION(imagecreatefromgif);
JONJ_FUNCTION(imagecreatefromjpeg);
JONJ_FUNCTION(imagecreatefromxbm);
JONJ_FUNCTION(imagecreatefromwebp);
JONJ_FUNCTION(imagecreatefrompng);
JONJ_FUNCTION(imagecreatefromwbmp);
JONJ_FUNCTION(imagecreatefromgd);
JONJ_FUNCTION(imagecreatefromgd2);
JONJ_FUNCTION(imagecreatefromgd2part);
#if defined(HAVE_GD_XPM)
JONJ_FUNCTION(imagecreatefromxpm);
#endif

JONJ_FUNCTION(imagegammacorrect);
JONJ_FUNCTION(imagedestroy);
JONJ_FUNCTION(imagefill);
JONJ_FUNCTION(imagefilledpolygon);
JONJ_FUNCTION(imagefilledrectangle);
JONJ_FUNCTION(imagefilltoborder);
JONJ_FUNCTION(imagefontwidth);
JONJ_FUNCTION(imagefontheight);

JONJ_FUNCTION(imagegif );
JONJ_FUNCTION(imagejpeg );
JONJ_FUNCTION(imagepng);
JONJ_FUNCTION(imagewebp);
JONJ_FUNCTION(imagewbmp);
JONJ_FUNCTION(imagegd);
JONJ_FUNCTION(imagegd2);

JONJ_FUNCTION(imageinterlace);
JONJ_FUNCTION(imageline);
JONJ_FUNCTION(imageloadfont);
JONJ_FUNCTION(imagepolygon);
JONJ_FUNCTION(imagerectangle);
JONJ_FUNCTION(imagesetpixel);
JONJ_FUNCTION(imagestring);
JONJ_FUNCTION(imagestringup);
JONJ_FUNCTION(imagesx);
JONJ_FUNCTION(imagesy);
JONJ_FUNCTION(imagedashedline);
JONJ_FUNCTION(imagettfbbox);
JONJ_FUNCTION(imagettftext);
JONJ_FUNCTION(imagepsloadfont);
/*
JONJ_FUNCTION(imagepscopyfont);
*/
JONJ_FUNCTION(imagepsfreefont);
JONJ_FUNCTION(imagepsencodefont);
JONJ_FUNCTION(imagepsextendfont);
JONJ_FUNCTION(imagepsslantfont);
JONJ_FUNCTION(imagepstext);
JONJ_FUNCTION(imagepsbbox);

JONJ_FUNCTION(jpeg2wbmp);
JONJ_FUNCTION(png2wbmp);
JONJ_FUNCTION(image2wbmp);

JONJ_FUNCTION(imagecolormatch);

JONJ_FUNCTION(imagelayereffect);
JONJ_FUNCTION(imagexbm);

JONJ_FUNCTION(imagefilter);
JONJ_FUNCTION(imageconvolution);

JONJ_GD_API int jonji_get_le_gd(void);

#else

#define jonjext_gd_ptr NULL

#endif

#endif /* JONJ_GD_H */
