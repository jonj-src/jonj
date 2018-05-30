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
   | Author: Frank M. Kromann <frank@kromann.info>                        |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef JONJ_MSSQL_H
#define JONJ_MSSQL_H


#if HAVE_MSSQL
#define MSDBLIB

/* FreeTDS checks for JONJ_MSSQL_API for avoid type redefinition */
#ifdef HAVE_FREETDS
#define JONJ_MSSQL_API
#endif

#include <sqlfront.h>
#include <sqldb.h>

typedef short TDS_SHORT;
#ifdef HAVE_FREETDS
#define MSSQL_VERSION "FreeTDS"
#define SQLTEXT SYBTEXT
#define SQLCHAR SYBCHAR
#define SQLVARCHAR SYBVARCHAR
#define SQLINT1 SYBINT1
#define SQLINT2 SYBINT2
#define SQLINT4 SYBINT4
#define SQLINTN SYBINTN
#define SQLBIT SYBBIT
#define SQLFLT4 SYBREAL
#define SQLFLT8 SYBFLT8
#define SQLFLTN SYBFLTN
#define SQLDECIMAL SYBDECIMAL
#define SQLNUMERIC SYBNUMERIC
#define SQLDATETIME SYBDATETIME
#define SQLDATETIM4 SYBDATETIME4
#define SQLDATETIMN SYBDATETIMN
#define SQLMONEY SYBMONEY
#define SQLMONEY4 SYBMONEY4
#define SQLMONEYN SYBMONEYN
#define SQLIMAGE SYBIMAGE
#define SQLBINARY SYBBINARY
#define SQLVARBINARY SYBVARBINARY
#ifdef SQLUNIQUE /* FreeTSD 0.61+ */
#define SQLUNIQUE SYBUNIQUE
#endif
#define DBERRHANDLE(a, b) dberrhandle(b)
#define DBMSGHANDLE(a, b) dbmsghandle(b)
#define DBSETOPT(a, b, c) dbsetopt(a, b, c, -1)
#define NO_MORE_RPC_RESULTS 3
#ifndef dbfreelogin
#define dbfreelogin dbloginfree
#endif
#define dbrpcexec dbrpcsend
typedef unsigned char	*LPBYTE;
typedef float           DBFLT4;
#else
#define MSSQL_VERSION "7.0"
#define DBERRHANDLE(a, b) dbprocerrhandle(a, b)
#define DBMSGHANDLE(a, b) dbprocmsghandle(a, b)
#define EHANDLEFUNC DBERRHANDLE_PROC
#define MHANDLEFUNC DBMSGHANDLE_PROC
#define DBSETOPT(a, b, c) dbsetopt(a, b, c)
#endif

#define coltype(j) dbcoltype(mssql_ptr->link,j)
#define intcol(i) ((int) *(DBINT *) dbdata(mssql_ptr->link,i))
#define smallintcol(i) ((int) *(DBSMALLINT *) dbdata(mssql_ptr->link,i))
#define tinyintcol(i) ((int) *(DBTINYINT *) dbdata(mssql_ptr->link,i))
#define anyintcol(j) (coltype(j)==SQLINT4?intcol(j):(coltype(j)==SQLINT2?smallintcol(j):tinyintcol(j)))
#define charcol(i) ((DBCHAR *) dbdata(mssql_ptr->link,i))
#define floatcol4(i) (*(DBFLT4 *) dbdata(mssql_ptr->link,i))
#define floatcol8(i) (*(DBFLT8 *) dbdata(mssql_ptr->link,i))

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry mssql_module_entry;
#define mssql_module_ptr &mssql_module_entry

JONJ_MINIT_FUNCTION(mssql);
JONJ_MSHUTDOWN_FUNCTION(mssql);
JONJ_RINIT_FUNCTION(mssql);
JONJ_RSHUTDOWN_FUNCTION(mssql);
JONJ_MINFO_FUNCTION(mssql);

JONJ_FUNCTION(mssql_connect);
JONJ_FUNCTION(mssql_pconnect);
JONJ_FUNCTION(mssql_close);
JONJ_FUNCTION(mssql_select_db);
JONJ_FUNCTION(mssql_query);
JONJ_FUNCTION(mssql_fetch_batch);
JONJ_FUNCTION(mssql_rows_affected);
JONJ_FUNCTION(mssql_free_result);
JONJ_FUNCTION(mssql_get_last_message);
JONJ_FUNCTION(mssql_num_rows);
JONJ_FUNCTION(mssql_num_fields);
JONJ_FUNCTION(mssql_fetch_field);
JONJ_FUNCTION(mssql_fetch_row);
JONJ_FUNCTION(mssql_fetch_array);
JONJ_FUNCTION(mssql_fetch_assoc);
JONJ_FUNCTION(mssql_fetch_object);
JONJ_FUNCTION(mssql_field_length);
JONJ_FUNCTION(mssql_field_name);
JONJ_FUNCTION(mssql_field_type);
JONJ_FUNCTION(mssql_data_seek);
JONJ_FUNCTION(mssql_field_seek);
JONJ_FUNCTION(mssql_result);
JONJ_FUNCTION(mssql_next_result);
JONJ_FUNCTION(mssql_min_error_severity);
JONJ_FUNCTION(mssql_min_message_severity);
JONJ_FUNCTION(mssql_init);
JONJ_FUNCTION(mssql_bind);
JONJ_FUNCTION(mssql_execute);
JONJ_FUNCTION(mssql_free_statement);
JONJ_FUNCTION(mssql_guid_string);

typedef struct mssql_link {
	LOGINREC *login;
	DBPROCESS *link;
	int valid;
} mssql_link;

typedef struct mssql_statement {
	int id;
	mssql_link *link;
	HashTable *binds;
	int executed;
} mssql_statement;

typedef struct {
	
	zval *zval;
	/* TODO: more data for special types (BLOBS, NUMERIC...) */
} mssql_bind;

ZEND_BEGIN_MODULE_GLOBALS(mssql)
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	zend_bool allow_persistent;
	char *appname;
	char *server_message;
#ifdef HAVE_FREETDS
	char *charset;
#endif
	long min_error_severity, min_message_severity;
	long cfg_min_error_severity, cfg_min_message_severity;
	long connect_timeout, timeout;
	zend_bool compatibility_mode;
	void (*get_column_content)(mssql_link *mssql_ptr,int offset,zval *result,int column_type  TSRMLS_DC);
	long textsize, textlimit, batchsize;
	zend_bool datetimeconvert;
	HashTable *resource_list, *resource_plist;
	zend_bool secure_connection;
	long max_procs;
ZEND_END_MODULE_GLOBALS(mssql)

#define MSSQL_ROWS_BLOCK 128

typedef struct mssql_field {
	char *name,*column_source;
	long max_length; 
	int numeric;
	int type;
} mssql_field;

typedef struct mssql_result {
	zval **data;
	mssql_field *fields;
	mssql_link *mssql_ptr;
	mssql_statement * statement;
	int batchsize;
	int lastresult;
	int blocks_initialized;
	int cur_row,cur_field;
	int num_rows,num_fields,have_fields;
} mssql_result;


#ifdef ZTS
# define MS_SQL_G(v) TSRMG(mssql_globals_id, zend_mssql_globals *, v)
#else
# define MS_SQL_G(v)	(mssql_globals.v)
#endif

#else

#define mssql_module_ptr NULL

#endif /* HAVE_MSSQL */

#define jonjext_mssql_ptr mssql_module_ptr

#endif /* JONJ_MSSQL_H */
