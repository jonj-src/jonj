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

#ifndef JONJ_WIN32_GLOBALS_H
#define JONJ_WIN32_GLOBALS_H

/* misc globals for thread-safety under win32 */

typedef struct _jonj_win32_core_globals jonj_win32_core_globals;

#ifdef ZTS
# define PW32G(v)		TSRMG(jonj_win32_core_globals_id, jonj_win32_core_globals*, v)
extern JONJAPI int jonj_win32_core_globals_id;
#else
# define PW32G(v)		(the_jonj_win32_core_globals.v)
extern JONJAPI struct _jonj_win32_core_globals the_jonj_win32_core_globals;
#endif

struct _jonj_win32_core_globals {
	/* syslog */
	char *log_header;
	HANDLE log_source;

	HKEY       registry_key;
	HANDLE     registry_event;
	HashTable *registry_directories;
};

void jonj_win32_core_globals_ctor(void *vg TSRMLS_DC);
void jonj_win32_core_globals_dtor(void *vg TSRMLS_DC);
JONJ_RSHUTDOWN_FUNCTION(win32_core_globals);

#endif

