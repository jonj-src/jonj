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

#if HAVE_EXIF
extern zend_module_entry exif_module_entry;
#define jonjext_exif_ptr &exif_module_entry

JONJ_FUNCTION(exif_read_data);
JONJ_FUNCTION(exif_tagname);
JONJ_FUNCTION(exif_thumbnail);
JONJ_FUNCTION(exif_imagetype);
#endif
