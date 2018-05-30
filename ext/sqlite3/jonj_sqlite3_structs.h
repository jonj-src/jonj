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

#ifndef JONJ_SQLITE_STRUCTS_H
#define JONJ_SQLITE_STRUCTS_H

#include <sqlite3.h>

/* for backwards compatibility reasons */
#ifndef SQLITE_OPEN_READONLY
#define SQLITE_OPEN_READONLY 0x00000001
#endif

#ifndef SQLITE_OPEN_READWRITE
#define SQLITE_OPEN_READWRITE 0x00000002
#endif

#ifndef SQLITE_OPEN_CREATE
#define SQLITE_OPEN_CREATE 0x00000004
#endif

/* Structure for SQLite Statement Parameter. */
struct jonj_sqlite3_bound_param  {
	long param_number;
	char *name;
	int name_len;
	long type;

	zval *parameter;
};

struct jonj_sqlite3_fci {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
};

/* Structure for SQLite function. */
typedef struct _jonj_sqlite3_func {
	struct _jonj_sqlite3_func *next;

	const char *func_name;
	int argc;

	zval *func, *step, *fini;
	struct jonj_sqlite3_fci afunc, astep, afini;
} jonj_sqlite3_func;

/* Structure for SQLite collation function */
typedef struct _jonj_sqlite3_collation {
	struct _jonj_sqlite3_collation *next;

	const char *collation_name;
	zval *cmp_func;
	struct jonj_sqlite3_fci fci;
} jonj_sqlite3_collation;

/* Structure for SQLite Database object. */
typedef struct _jonj_sqlite3_db_object  {
	zend_object zo;
	int initialised;
	sqlite3 *db;
	jonj_sqlite3_func *funcs;
	jonj_sqlite3_collation *collations;

	zend_bool exception;

	zend_llist free_list;
} jonj_sqlite3_db_object;

/* Structure for SQLite Database object. */
typedef struct _jonj_sqlite3_agg_context  {
	zval *zval_context;
	long row_count;
} jonj_sqlite3_agg_context;

typedef struct _jonj_sqlite3_stmt_object jonj_sqlite3_stmt;
typedef struct _jonj_sqlite3_result_object jonj_sqlite3_result;

/* sqlite3 objects to be destroyed */
typedef struct _jonj_sqlite3_free_list {
	zval *stmt_obj_zval;
	jonj_sqlite3_stmt *stmt_obj;
} jonj_sqlite3_free_list;

/* Structure for SQLite Result object. */
struct _jonj_sqlite3_result_object  {
	zend_object zo;
	jonj_sqlite3_db_object *db_obj;
	jonj_sqlite3_stmt *stmt_obj;
	zval *stmt_obj_zval;
	
	int is_prepared_statement;
	int complete;
};

/* Structure for SQLite Statement object. */
struct _jonj_sqlite3_stmt_object  {
	zend_object zo;
	sqlite3_stmt *stmt;
	jonj_sqlite3_db_object *db_obj;
	zval *db_obj_zval;

	int initialised;

	/* Keep track of the zvals for bound parameters */
	HashTable *bound_params;
};

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
