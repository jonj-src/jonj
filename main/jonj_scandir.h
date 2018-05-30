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
   | Authors: Shane Caraveo <shane@caraveo.com>                           |
   |          Ilia Alshanetsky  <ilia@prohost.org>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SCANDIR_H
#define JONJ_SCANDIR_H

#include <sys/types.h>

#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif

#ifdef JONJ_WIN32
#include "config.w32.h"
#include "win32/readdir.h"
#else
#include <jonj_config.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef HAVE_SCANDIR
#define jonj_scandir		scandir
#else
JONJAPI int jonj_scandir(const char *dirname, struct dirent **namelist[], int (*selector) (const struct dirent *entry), int (*compare) (const struct dirent **a, const struct dirent **b));
#endif

#ifdef HAVE_ALPHASORT
#define jonj_alphasort	alphasort
#else
JONJAPI int jonj_alphasort(const struct dirent **a, const struct dirent **b);
#endif

#endif /* JONJ_SCANDIR_H */
