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
   |          Marcus Boerger <helly@jonj.tk>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_IMAGE_H
#define JONJ_IMAGE_H

JONJ_FUNCTION(getimagesize);
JONJ_FUNCTION(getimagesizefromstring);

JONJ_FUNCTION(image_type_to_mime_type);
JONJ_FUNCTION(image_type_to_extension);

/* {{{ enum image_filetype
   This enum is used to have ext/standard/image.c and ext/exif/exif.c use
   the same constants for file types.
*/
typedef enum
{ IMAGE_FILETYPE_UNKNOWN=0,
  IMAGE_FILETYPE_GIF=1,
  IMAGE_FILETYPE_JPEG,
  IMAGE_FILETYPE_PNG,
  IMAGE_FILETYPE_SWF,
  IMAGE_FILETYPE_PSD,
  IMAGE_FILETYPE_BMP,
  IMAGE_FILETYPE_TIFF_II, /* intel */
  IMAGE_FILETYPE_TIFF_MM, /* motorola */
  IMAGE_FILETYPE_JPC,
  IMAGE_FILETYPE_JP2,
  IMAGE_FILETYPE_JPX,
  IMAGE_FILETYPE_JB2,
  IMAGE_FILETYPE_SWC,
  IMAGE_FILETYPE_IFF,
  IMAGE_FILETYPE_WBMP,
  /* IMAGE_FILETYPE_JPEG2000 is a userland alias for IMAGE_FILETYPE_JPC */
  IMAGE_FILETYPE_XBM,
  IMAGE_FILETYPE_ICO,
/* WHEN EXTENDING: PLEASE ALSO REGISTER IN image.c:JONJ_MINIT_FUNCTION(imagetypes) */
  IMAGE_FILETYPE_COUNT
} image_filetype;
/* }}} */

JONJ_MINIT_FUNCTION(imagetypes);

JONJAPI int jonj_getimagetype(jonj_stream *stream, char *filetype TSRMLS_DC);

JONJAPI char * jonj_image_type_to_mime_type(int image_type);

#endif /* JONJ_IMAGE_H */
