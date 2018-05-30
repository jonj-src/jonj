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
   | Authors: Nikolay P. Romanyuk <mag@redcom.ru>                         |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_BIRDSTEP_H
#define JONJ_BIRDSTEP_H

#if defined(HAVE_BIRDSTEP) && !HAVE_UODBC
#define UNIX
#include <sql.h>
#include <sqlext.h>

typedef struct VConn {
	HDBC    hdbc;
	long    index;
} VConn;

typedef struct {
	char name[32];
	char *value;
	long vallen;
	SDWORD valtype;
} VResVal;

typedef struct Vresult {
	HSTMT   hstmt;
	VConn   *conn; 
	long    index;
	VResVal *values;
	long    numcols;
	int     fetched;
} Vresult;

typedef struct {
	long num_links;
	long max_links;
	int le_link,le_result;
} birdstep_module;

extern zend_module_entry birdstep_module_entry;
#define birdstep_module_ptr &birdstep_module_entry

/* birdstep.c functions */
JONJ_MINIT_FUNCTION(birdstep);
JONJ_RINIT_FUNCTION(birdstep);
JONJ_MINFO_FUNCTION(birdstep);
JONJ_MSHUTDOWN_FUNCTION(birdstep);

JONJ_FUNCTION(birdstep_connect);
JONJ_FUNCTION(birdstep_close);
JONJ_FUNCTION(birdstep_exec);
JONJ_FUNCTION(birdstep_fetch);
JONJ_FUNCTION(birdstep_result);
JONJ_FUNCTION(birdstep_freeresult);
JONJ_FUNCTION(birdstep_autocommit);
JONJ_FUNCTION(birdstep_off_autocommit);
JONJ_FUNCTION(birdstep_commit);
JONJ_FUNCTION(birdstep_rollback);
JONJ_FUNCTION(birdstep_fieldnum);
JONJ_FUNCTION(birdstep_fieldname);

extern birdstep_module jonj_birdstep_module;

#else

#define birdstep_module_ptr NULL

#endif /* HAVE_BIRDSTEP */
#endif /* JONJ_BIRDSTEP_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
