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
	long setup_env;
	long current_hook;
	zend_bool in_request;
	zend_bool apache_config_loaded;
	zend_bool headers_sent;
} jonj_apache_info_struct;

typedef struct _jonj_handler {
    long type;
    long stage;
    char *name;
} jonj_handler;

#define AP_HANDLER_TYPE_FILE 0
#define AP_HANDLER_TYPE_METHOD 1

extern zend_module_entry apache_module_entry;

#ifdef ZTS
extern int jonj_apache_info_id;
#define AP(v) TSRMG(jonj_apache_info_id, jonj_apache_info_struct *, v)
#else
extern jonj_apache_info_struct jonj_apache_info;
#define AP(v) (jonj_apache_info.v)
#endif

/* defines for the various stages of the apache request */
#define AP_WAITING_FOR_REQUEST 0
#define AP_POST_READ 1
#define AP_URI_TRANS 2
#define AP_HEADER_PARSE 3
#define AP_ACCESS_CONTROL 4
#define AP_AUTHENTICATION 5
#define AP_AUTHORIZATION 6
#define AP_TYPE_CHECKING 7
#define AP_FIXUP 8
#define AP_RESPONSE 9
#define AP_LOGGING 10
#define AP_CLEANUP 11


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
