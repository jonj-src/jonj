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
   | Author: Wez Furlong <wez@jonj.tk>                                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj.h"
#include "jonj_win32_globals.h"
#include "syslog.h"

#ifdef ZTS
JONJAPI int jonj_win32_core_globals_id;
#else
jonj_win32_core_globals the_jonj_win32_core_globals;
#endif

void jonj_win32_core_globals_ctor(void *vg TSRMLS_DC)
{
	jonj_win32_core_globals *wg = (jonj_win32_core_globals*)vg;
	memset(wg, 0, sizeof(*wg));
}

void jonj_win32_core_globals_dtor(void *vg TSRMLS_DC)
{
	jonj_win32_core_globals *wg = (jonj_win32_core_globals*)vg;

	if (wg->registry_key) {
		RegCloseKey(wg->registry_key);
		wg->registry_key = NULL;
	}
	if (wg->registry_event) {
		CloseHandle(wg->registry_event);
		wg->registry_event = NULL;
	}
	if (wg->registry_directories) {
		zend_hash_destroy(wg->registry_directories);
		free(wg->registry_directories);
		wg->registry_directories = NULL;
	}
}


JONJ_RSHUTDOWN_FUNCTION(win32_core_globals)
{
	jonj_win32_core_globals *wg =
#ifdef ZTS
		ts_resource(jonj_win32_core_globals_id)
#else
		&the_jonj_win32_core_globals
#endif
		;

	closelog();

	return SUCCESS;
}

