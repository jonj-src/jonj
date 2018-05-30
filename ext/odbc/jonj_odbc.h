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
   | Authors: Stig Sæther Bakken <ssb@jonj.tk>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |	      Kevin N. Shallow <kshallow@tampabay.rr.com> (Birdstep)      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_ODBC_H
#define JONJ_ODBC_H

#if HAVE_UODBC

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry odbc_module_entry;
#define odbc_module_ptr &odbc_module_entry

#if defined(HAVE_DBMAKER) || defined(JONJ_WIN32) || defined(HAVE_IBMDB2) || defined(HAVE_UNIXODBC) || defined(HAVE_BIRDSTEP) || defined(HAVE_IODBC)
# define JONJ_ODBC_HAVE_FETCH_HASH 1
#endif

/* user functions */
JONJ_MINIT_FUNCTION(odbc);
JONJ_MSHUTDOWN_FUNCTION(odbc);
JONJ_RINIT_FUNCTION(odbc);
JONJ_RSHUTDOWN_FUNCTION(odbc);
JONJ_MINFO_FUNCTION(odbc);

JONJ_FUNCTION(odbc_error);
JONJ_FUNCTION(odbc_errormsg);
JONJ_FUNCTION(odbc_setoption);
JONJ_FUNCTION(odbc_autocommit);
JONJ_FUNCTION(odbc_close);
JONJ_FUNCTION(odbc_close_all);
JONJ_FUNCTION(odbc_commit);
JONJ_FUNCTION(odbc_connect);
JONJ_FUNCTION(odbc_pconnect);
JONJ_FUNCTION(odbc_cursor);
#ifdef HAVE_SQLDATASOURCES
JONJ_FUNCTION(odbc_data_source);
#endif
JONJ_FUNCTION(odbc_do);
JONJ_FUNCTION(odbc_exec);
JONJ_FUNCTION(odbc_execute);
#ifdef JONJ_ODBC_HAVE_FETCH_HASH
JONJ_FUNCTION(odbc_fetch_array);
JONJ_FUNCTION(odbc_fetch_object);
#endif
JONJ_FUNCTION(odbc_fetch_into);
JONJ_FUNCTION(odbc_fetch_row);
JONJ_FUNCTION(odbc_field_len);
JONJ_FUNCTION(odbc_field_scale);
JONJ_FUNCTION(odbc_field_name);
JONJ_FUNCTION(odbc_field_type);
JONJ_FUNCTION(odbc_field_num);
JONJ_FUNCTION(odbc_free_result);
#if !defined(HAVE_SOLID) && !defined(HAVE_SOLID_30)
JONJ_FUNCTION(odbc_next_result);
#endif
JONJ_FUNCTION(odbc_num_fields);
JONJ_FUNCTION(odbc_num_rows);
JONJ_FUNCTION(odbc_prepare);
JONJ_FUNCTION(odbc_result);
JONJ_FUNCTION(odbc_result_all);
JONJ_FUNCTION(odbc_rollback);
JONJ_FUNCTION(odbc_binmode);
JONJ_FUNCTION(odbc_longreadlen);
JONJ_FUNCTION(odbc_tables);
JONJ_FUNCTION(odbc_columns);
#if !defined(HAVE_DBMAKER) && !defined(HAVE_SOLID) && !defined(HAVE_SOLID_35)    /* not supported now */
JONJ_FUNCTION(odbc_columnprivileges);
JONJ_FUNCTION(odbc_tableprivileges);
#endif
#if !defined(HAVE_SOLID) || !defined(HAVE_SOLID_35)    /* not supported */
JONJ_FUNCTION(odbc_foreignkeys);
JONJ_FUNCTION(odbc_procedures);
JONJ_FUNCTION(odbc_procedurecolumns);
#endif
JONJ_FUNCTION(odbc_gettypeinfo);
JONJ_FUNCTION(odbc_primarykeys);
JONJ_FUNCTION(odbc_specialcolumns);
JONJ_FUNCTION(odbc_statistics);

#else

#define odbc_module_ptr NULL

#endif /* HAVE_UODBC */

#define jonjext_odbc_ptr odbc_module_ptr

#endif /* JONJ_ODBC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
