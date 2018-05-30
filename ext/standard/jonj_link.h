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
   | Author:                                                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_LINK_H
#define JONJ_LINK_H

#if defined(HAVE_SYMLINK) || defined(JONJ_WIN32)

JONJ_FUNCTION(link);
JONJ_FUNCTION(readlink);
JONJ_FUNCTION(linkinfo);
JONJ_FUNCTION(symlink);

#endif

#endif /* JONJ_LINK_H */
