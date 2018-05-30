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
   | Author: Kristian Koehntopp <kris@koehntopp.de>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef JONJ_POSIX_H
#define JONJ_POSIX_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_POSIX
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry posix_module_entry;
#define posix_module_ptr &posix_module_entry

/* POSIX.1, 3.3 */
JONJ_FUNCTION(posix_kill);

/* POSIX.1, 4.1 */
JONJ_FUNCTION(posix_getpid);
JONJ_FUNCTION(posix_getppid);

/* POSIX.1,  4.2 */
JONJ_FUNCTION(posix_getuid);
JONJ_FUNCTION(posix_getgid);
JONJ_FUNCTION(posix_geteuid);
JONJ_FUNCTION(posix_getegid);
JONJ_FUNCTION(posix_setuid);
JONJ_FUNCTION(posix_setgid);
#ifdef HAVE_SETEUID
JONJ_FUNCTION(posix_seteuid);
#endif
#ifdef HAVE_SETEGID
JONJ_FUNCTION(posix_setegid);
#endif
#ifdef HAVE_GETGROUPS
JONJ_FUNCTION(posix_getgroups);
#endif
#ifdef HAVE_GETLOGIN
JONJ_FUNCTION(posix_getlogin);
#endif

/* POSIX.1, 4.3 */
JONJ_FUNCTION(posix_getpgrp);
#ifdef HAVE_SETSID
JONJ_FUNCTION(posix_setsid);
#endif
JONJ_FUNCTION(posix_setpgid);
/* Non-Posix functions which are common */
#ifdef HAVE_GETPGID
JONJ_FUNCTION(posix_getpgid);
#endif
#ifdef HAVE_GETSID
JONJ_FUNCTION(posix_getsid);
#endif

/* POSIX.1, 4.4 */
JONJ_FUNCTION(posix_uname);
JONJ_FUNCTION(posix_times);

/* POSIX.1, 4.5 */
#ifdef HAVE_CTERMID
JONJ_FUNCTION(posix_ctermid);
#endif
JONJ_FUNCTION(posix_ttyname);
JONJ_FUNCTION(posix_isatty);

/* POSIX.1, 5.2 */
JONJ_FUNCTION(posix_getcwd);

/* POSIX.1, 5.4 */
#ifdef HAVE_MKFIFO
JONJ_FUNCTION(posix_mkfifo);
#endif
#ifdef HAVE_MKNOD
JONJ_FUNCTION(posix_mknod);
#endif

/* POSIX.1, 5.6 */
JONJ_FUNCTION(posix_access);

/* POSIX.1, 9.2 */
JONJ_FUNCTION(posix_getgrnam);
JONJ_FUNCTION(posix_getgrgid);
JONJ_FUNCTION(posix_getpwnam);
JONJ_FUNCTION(posix_getpwuid);

#ifdef HAVE_GETRLIMIT
JONJ_FUNCTION(posix_getrlimit);
#endif

#ifdef HAVE_INITGROUPS
JONJ_FUNCTION(posix_initgroups);
#endif

JONJ_FUNCTION(posix_get_last_error);
JONJ_FUNCTION(posix_strerror);

ZEND_BEGIN_MODULE_GLOBALS(posix)
	int last_error;
ZEND_END_MODULE_GLOBALS(posix)

#ifdef ZTS
# define POSIX_G(v) TSRMG(posix_globals_id, zend_posix_globals *, v)
#else
# define POSIX_G(v)	(posix_globals.v)
#endif

#else

#define posix_module_ptr NULL

#endif

#define jonjext_posix_ptr posix_module_ptr

#endif /* JONJ_POSIX_H */
