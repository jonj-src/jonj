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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef FLOCK_COMPAT_H
#define FLOCK_COMPAT_H

/* jonj_flock internally uses fcntl whther or not flock is available
 * This way our jonj_flock even works on NFS files.
 * More info: /usr/src/linux/Documentation
 */
JONJAPI int jonj_flock(int fd, int operation);

#ifndef HAVE_FLOCK
#	define LOCK_SH 1
#	define LOCK_EX 2
#	define LOCK_NB 4
#	define LOCK_UN 8
JONJAPI int flock(int fd, int operation);
#endif

/* Userland LOCK_* constants */
#define JONJ_LOCK_SH 1
#define JONJ_LOCK_EX 2
#define JONJ_LOCK_UN 3
#define JONJ_LOCK_NB 4

#ifdef JONJ_WIN32
# ifdef EWOULDBLOCK
#  undef EWOULDBLOCK
# endif
# define EWOULDBLOCK WSAEWOULDBLOCK
# define fsync _commit
# define ftruncate(a, b) chsize(a, b)
#endif /* defined(JONJ_WIN32) */

#if !HAVE_INET_ATON
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef JONJ_WIN32
extern int inet_aton(const char *, struct in_addr *); 
#endif
#endif

#endif	/* FLOCK_COMPAT_H */
