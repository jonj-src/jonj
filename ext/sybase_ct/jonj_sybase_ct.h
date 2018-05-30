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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   |          Timm Friebe <jonj_sybase_ct@thekid.de>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SYBASE_CT_H
#define JONJ_SYBASE_CT_H

#if HAVE_SYBASE_CT

#define CTLIB_VERSION CS_VERSION_100

extern zend_module_entry sybase_module_entry;
#define sybase_module_ptr &sybase_module_entry

JONJ_MINIT_FUNCTION(sybase);
JONJ_MSHUTDOWN_FUNCTION(sybase);
JONJ_RINIT_FUNCTION(sybase);
JONJ_RSHUTDOWN_FUNCTION(sybase);
JONJ_MINFO_FUNCTION(sybase);

JONJ_FUNCTION(sybase_connect);
JONJ_FUNCTION(sybase_pconnect);
JONJ_FUNCTION(sybase_close);
JONJ_FUNCTION(sybase_select_db);
JONJ_FUNCTION(sybase_query);
JONJ_FUNCTION(sybase_unbuffered_query);
JONJ_FUNCTION(sybase_free_result);
JONJ_FUNCTION(sybase_get_last_message);
JONJ_FUNCTION(sybase_num_rows);
JONJ_FUNCTION(sybase_num_fields);
JONJ_FUNCTION(sybase_fetch_row);
JONJ_FUNCTION(sybase_fetch_array);
JONJ_FUNCTION(sybase_fetch_assoc);
JONJ_FUNCTION(sybase_fetch_object);
JONJ_FUNCTION(sybase_data_seek);
JONJ_FUNCTION(sybase_result);
JONJ_FUNCTION(sybase_affected_rows);
JONJ_FUNCTION(sybase_field_seek);
JONJ_FUNCTION(sybase_min_client_severity);
JONJ_FUNCTION(sybase_min_server_severity);
JONJ_FUNCTION(sybase_fetch_field);
JONJ_FUNCTION(sybase_set_message_handler);
JONJ_FUNCTION(sybase_deadlock_retry_count);

#include <ctpublic.h>

ZEND_BEGIN_MODULE_GLOBALS(sybase)
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long login_timeout;
	long allow_persistent;
	char *appname;
	char *hostname;
	char *server_message;
	long min_server_severity, min_client_severity;
	long deadlock_retry_count;
	zval *callback_name;
	CS_CONTEXT *context;
ZEND_END_MODULE_GLOBALS(sybase)

typedef struct {
	CS_CONNECTION *connection;
	CS_COMMAND *cmd;
	int valid;
	int deadlock;
	int dead;
	int active_result_index;
	long affected_rows;
	zval *callback_name;
} sybase_link;

#define SYBASE_ROWS_BLOCK 128

typedef struct {
	char *name,*column_source;
	int max_length, numeric;
	CS_INT type;
} sybase_field;

typedef struct {
	zval **data;
	sybase_field *fields;
	sybase_link *sybase_ptr;
	int cur_row,cur_field;
	int num_rows,num_fields;
	
	/* For unbuffered reads */
	CS_INT *lengths;
	CS_SMALLINT *indicators;
	char **tmp_buffer;
	unsigned char *numerics;
	CS_INT *types;
	CS_DATAFMT *datafmt;
	int blocks_initialized;
	CS_RETCODE last_retcode;
	int store;
} sybase_result;

#ifdef ZTS
# define SybCtG(v) TSRMG(sybase_globals_id, zend_sybase_globals *, v)
#else
# define SybCtG(v) (sybase_globals.v)
#endif

#else

#define sybase_module_ptr NULL

#endif

#define jonjext_sybase_ct_ptr sybase_module_ptr

#endif /* JONJ_SYBASE_CT_H */
