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
   | Author: Stig S�ther Bakken <ssb@jonj.tk>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "jonj.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>
#ifdef JONJ_WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include "jonj_lcg.h"
#include "uniqid.h"

/* {{{ proto string uniqid([string prefix [, bool more_entropy]])
   Generates a unique ID */
#ifdef HAVE_GETTIMEOFDAY
JONJ_FUNCTION(uniqid)
{
	char *prefix = "";
#if defined(__CYGWIN__)
	zend_bool more_entropy = 1;
#else
	zend_bool more_entropy = 0;
#endif
	char *uniqid;
	int sec, usec, prefix_len = 0;
	struct timeval tv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sb", &prefix, &prefix_len,
							  &more_entropy)) {
		return;
	}

#if HAVE_USLEEP && !defined(JONJ_WIN32)
	if (!more_entropy) {
#if defined(__CYGWIN__)
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "You must use 'more entropy' under CYGWIN");
		RETURN_FALSE;
#else
		usleep(1);
#endif
	}
#endif
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 0x100000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs.
	 */
	if (more_entropy) {
		spprintf(&uniqid, 0, "%s%08x%05x%.8F", prefix, sec, usec, jonj_combined_lcg(TSRMLS_C) * 10);
	} else {
		spprintf(&uniqid, 0, "%s%08x%05x", prefix, sec, usec);
	}

	RETURN_STRING(uniqid, 0);
}
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */