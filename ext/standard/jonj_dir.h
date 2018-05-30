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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_DIR_H
#define JONJ_DIR_H

/* directory functions */
JONJ_MINIT_FUNCTION(dir);
JONJ_RINIT_FUNCTION(dir);
JONJ_FUNCTION(opendir);
JONJ_FUNCTION(closedir);
JONJ_FUNCTION(chdir);
#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
JONJ_FUNCTION(chroot);
#endif
JONJ_FUNCTION(getcwd);
JONJ_FUNCTION(rewinddir);
JONJ_NAMED_FUNCTION(jonj_if_readdir);
JONJ_FUNCTION(getdir);
JONJ_FUNCTION(glob);
JONJ_FUNCTION(scandir);

#define JONJ_SCANDIR_SORT_ASCENDING 0
#define JONJ_SCANDIR_SORT_DESCENDING 1
#define JONJ_SCANDIR_SORT_NONE 2

#endif /* JONJ_DIR_H */
