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
   | Author: Jim Winstead <jimw@jonj.tk>                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj.h"
#include "pageinfo.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdlib.h>
#if HAVE_PWD_H
#ifdef JONJ_WIN32
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if HAVE_GRP_H
# ifdef JONJ_WIN32
#  include "win32/grp.h"
# else
#  include <grp.h>
# endif
#endif
#ifdef JONJ_WIN32
#undef getgid
#define getgroups(a, b) 0
#define getgid() 1
#define getuid() 1
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifdef JONJ_WIN32
#include <process.h>
#endif

#include "ext/standard/basic_functions.h"

/* {{{ jonj_statpage
 */
JONJAPI void jonj_statpage(TSRMLS_D)
{
	struct stat *pstat;

	pstat = sapi_get_stat(TSRMLS_C);

	if (BG(page_uid)==-1 || BG(page_gid)==-1) {
		if(pstat) {
			BG(page_uid)   = pstat->st_uid;
			BG(page_gid)   = pstat->st_gid;
			BG(page_inode) = pstat->st_ino;
			BG(page_mtime) = pstat->st_mtime;
		} else { /* handler for situations where there is no source file, ex. jonj -r */
			BG(page_uid) = getuid();
			BG(page_gid) = getgid();
		}
	}
}
/* }}} */

/* {{{ jonj_getuid
 */
long jonj_getuid(TSRMLS_D)
{
	jonj_statpage(TSRMLS_C);
	return (BG(page_uid));
}
/* }}} */

long jonj_getgid(TSRMLS_D)
{
	jonj_statpage(TSRMLS_C);
	return (BG(page_gid));
}

/* {{{ proto int getmyuid(void)
   Get JONJ script owner's UID */
JONJ_FUNCTION(getmyuid)
{
	long uid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	uid = jonj_getuid(TSRMLS_C);
	if (uid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(uid);
	}
}
/* }}} */

/* {{{ proto int getmygid(void)
   Get JONJ script owner's GID */
JONJ_FUNCTION(getmygid)
{
	long gid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	gid = jonj_getgid(TSRMLS_C);
	if (gid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(gid);
	}
}
/* }}} */

/* {{{ proto int getmypid(void)
   Get current process ID */
JONJ_FUNCTION(getmypid)
{
	int pid;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	pid = getpid();
	if (pid < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG((long) pid);
	}
}
/* }}} */

/* {{{ proto int getmyinode(void)
   Get the inode of the current script being parsed */
JONJ_FUNCTION(getmyinode)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	jonj_statpage(TSRMLS_C);
	if (BG(page_inode) < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(BG(page_inode));
	}
}
/* }}} */

JONJAPI long jonj_getlastmod(TSRMLS_D)
{
	jonj_statpage(TSRMLS_C);
	return BG(page_mtime);
}

/* {{{ proto int getlastmod(void)
   Get time of last page modification */
JONJ_FUNCTION(getlastmod)
{
	long lm;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	lm = jonj_getlastmod(TSRMLS_C);
	if (lm < 0) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(lm);
	}
}
/* }}} */

/*nma
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
