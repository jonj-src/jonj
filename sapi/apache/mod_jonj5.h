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
   | Author: Rasmus Lerdorf <rasmus@jonj.tk>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef MOD_JONJ5_H
#define MOD_JONJ5_H

#if !defined(WIN32) && !defined(WINNT)
#ifndef MODULE_VAR_EXPORT
#define MODULE_VAR_EXPORT
#endif
#endif

typedef struct {
	long engine;
	long last_modified;
	long xbithack;
	long terminate_child;
	zend_bool in_request;
} jonj_apache_info_struct;

extern zend_module_entry apache_module_entry;

#ifdef ZTS
extern int jonj_apache_info_id;
#define AP(v) TSRMG(jonj_apache_info_id, jonj_apache_info_struct *, v)
#else
extern jonj_apache_info_struct jonj_apache_info;
#define AP(v) (jonj_apache_info.v)
#endif

/* fix for gcc4 visibility patch */
#ifndef JONJ_WIN32
# undef MODULE_VAR_EXPORT
# define MODULE_VAR_EXPORT JONJAPI
#endif

#endif							/* MOD_JONJ5_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
