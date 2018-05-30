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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   |         Andrey Hristov <andrey@jonj.tk>                              |
   +----------------------------------------------------------------------+
*/


/* $Id$ */

#ifndef JONJ_MYSQL_STRUCTS_H
#define JONJ_MYSQL_STRUCTS_H

#ifdef ZTS
#include "TSRM.h"
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if defined(MYSQL_USE_MYSQLND)
#include "ext/mysqlnd/mysqlnd.h"
#include "mysql_mysqlnd.h"
#else
#include <mysql.h>
#endif

#ifdef JONJ_MYSQL_UNIX_SOCK_ADDR
#ifdef MYSQL_UNIX_ADDR
#undef MYSQL_UNIX_ADDR
#endif
#define MYSQL_UNIX_ADDR JONJ_MYSQL_UNIX_SOCK_ADDR
#endif

#if (MYSQL_VERSION_ID >= 40113 && MYSQL_VERSION_ID < 50000) || MYSQL_VERSION_ID >= 50007 || defined(MYSQL_USE_MYSQLND)
#define MYSQL_HAS_SET_CHARSET
#endif

JONJ_MINIT_FUNCTION(mysql);
JONJ_RINIT_FUNCTION(mysql);
JONJ_MSHUTDOWN_FUNCTION(mysql);
JONJ_RSHUTDOWN_FUNCTION(mysql);
JONJ_MINFO_FUNCTION(mysql);

JONJ_FUNCTION(mysql_connect);
JONJ_FUNCTION(mysql_pconnect);
JONJ_FUNCTION(mysql_close);
JONJ_FUNCTION(mysql_select_db);
#if MYSQL_VERSION_ID < 40000
JONJ_FUNCTION(mysql_create_db);
JONJ_FUNCTION(mysql_drop_db);
#endif
JONJ_FUNCTION(mysql_query);
JONJ_FUNCTION(mysql_unbuffered_query);
JONJ_FUNCTION(mysql_db_query);
JONJ_FUNCTION(mysql_list_dbs);
JONJ_FUNCTION(mysql_list_tables);
JONJ_FUNCTION(mysql_list_fields);
JONJ_FUNCTION(mysql_list_processes);
JONJ_FUNCTION(mysql_error);
JONJ_FUNCTION(mysql_errno);
JONJ_FUNCTION(mysql_affected_rows);
JONJ_FUNCTION(mysql_insert_id);
JONJ_FUNCTION(mysql_result);
JONJ_FUNCTION(mysql_num_rows);
JONJ_FUNCTION(mysql_num_fields);
JONJ_FUNCTION(mysql_fetch_row);
JONJ_FUNCTION(mysql_fetch_array);
JONJ_FUNCTION(mysql_fetch_assoc);
JONJ_FUNCTION(mysql_fetch_object);
JONJ_FUNCTION(mysql_data_seek);
JONJ_FUNCTION(mysql_fetch_lengths);
JONJ_FUNCTION(mysql_fetch_field);
JONJ_FUNCTION(mysql_field_seek);
JONJ_FUNCTION(mysql_free_result);
JONJ_FUNCTION(mysql_field_name);
JONJ_FUNCTION(mysql_field_table);
JONJ_FUNCTION(mysql_field_len);
JONJ_FUNCTION(mysql_field_type);
JONJ_FUNCTION(mysql_field_flags);
JONJ_FUNCTION(mysql_escape_string);
JONJ_FUNCTION(mysql_real_escape_string);
JONJ_FUNCTION(mysql_get_client_info);
JONJ_FUNCTION(mysql_get_host_info);
JONJ_FUNCTION(mysql_get_proto_info);
JONJ_FUNCTION(mysql_get_server_info);
JONJ_FUNCTION(mysql_info);
JONJ_FUNCTION(mysql_stat);
JONJ_FUNCTION(mysql_thread_id);
JONJ_FUNCTION(mysql_client_encoding);
JONJ_FUNCTION(mysql_ping);
#ifdef MYSQL_HAS_SET_CHARSET
JONJ_FUNCTION(mysql_set_charset);
#endif

ZEND_BEGIN_MODULE_GLOBALS(mysql)
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	long default_port;
	char *default_host, *default_user, *default_password;
	char *default_socket;
	char *connect_error;
	long connect_errno;
	long connect_timeout;
	long result_allocated;
	long trace_mode;
	long allow_local_infile;
ZEND_END_MODULE_GLOBALS(mysql)

#ifdef ZTS
# define MySG(v) TSRMG(mysql_globals_id, zend_mysql_globals *, v)
#else
# define MySG(v) (mysql_globals.v)
#endif


#endif /* JONJ_MYSQL_STRUCTS_H */
