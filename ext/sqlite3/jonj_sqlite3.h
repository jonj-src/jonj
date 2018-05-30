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
   | Authors: Scott MacVicar <scottmac@jonj.tk>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SQLITE3_H
#define JONJ_SQLITE3_H

#define JONJ_SQLITE3_VERSION	 "0.7-dev"

extern zend_module_entry sqlite3_module_entry;
#define jonjext_sqlite3_ptr &sqlite3_module_entry

ZEND_BEGIN_MODULE_GLOBALS(sqlite3)
	char *extension_dir;
ZEND_END_MODULE_GLOBALS(sqlite3)

#ifdef ZTS
# define SQLITE3G(v) TSRMG(sqlite3_globals_id, zend_sqlite3_globals *, v)
#else
# define SQLITE3G(v) (sqlite3_globals.v)
#endif

#define JONJ_SQLITE3_ASSOC	1<<0
#define JONJ_SQLITE3_NUM		1<<1
#define JONJ_SQLITE3_BOTH	(JONJ_SQLITE3_ASSOC|JONJ_SQLITE3_NUM)

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
