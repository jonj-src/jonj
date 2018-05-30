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
   |          Jouni Ahto <jouni.ahto@exdec.fi>                            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_PGSQL_H
#define JONJ_PGSQL_H

#if HAVE_PGSQL

#define JONJ_PGSQL_API_VERSION 20140217

extern zend_module_entry pgsql_module_entry;
#define pgsql_module_ptr &pgsql_module_entry

#ifdef JONJ_PGSQL_PRIVATE
#undef SOCKET_SIZE_TYPE
#include <libpq-fe.h>

#ifdef JONJ_WIN32
#define INV_WRITE            0x00020000
#define INV_READ             0x00040000
#undef JONJ_PGSQL_API
#ifdef PGSQL_EXPORTS
#define JONJ_PGSQL_API __declspec(dllexport)
#else
#define JONJ_PGSQL_API __declspec(dllimport)
#endif
#else
#include <libpq/libpq-fs.h>
# if defined(__GNUC__) && __GNUC__ >= 4
#  define JONJ_PGSQL_API __attribute__ ((visibility("default")))
# else
#  define JONJ_PGSQL_API
# endif
#endif

#ifdef HAVE_PG_CONFIG_H
#include <pg_config.h>
#endif

#ifdef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT
const char * pg_encoding_to_char(int encoding);
#endif

JONJ_MINIT_FUNCTION(pgsql);
JONJ_MSHUTDOWN_FUNCTION(pgsql);
JONJ_RINIT_FUNCTION(pgsql);
JONJ_RSHUTDOWN_FUNCTION(pgsql);
JONJ_MINFO_FUNCTION(pgsql);
/* connection functions */
JONJ_FUNCTION(pg_connect);
JONJ_FUNCTION(pg_pconnect);
JONJ_FUNCTION(pg_connect_poll);
JONJ_FUNCTION(pg_close);
JONJ_FUNCTION(pg_connection_reset);
JONJ_FUNCTION(pg_connection_status);
JONJ_FUNCTION(pg_connection_busy);
JONJ_FUNCTION(pg_host);
JONJ_FUNCTION(pg_dbname);
JONJ_FUNCTION(pg_port);
JONJ_FUNCTION(pg_tty);
JONJ_FUNCTION(pg_options);
JONJ_FUNCTION(pg_version);
JONJ_FUNCTION(pg_ping);
#if HAVE_PQPARAMETERSTATUS
JONJ_FUNCTION(pg_parameter_status);
#endif
#if HAVE_PGTRANSACTIONSTATUS
JONJ_FUNCTION(pg_transaction_status);
#endif
/* query functions */
JONJ_FUNCTION(pg_query);
#if HAVE_PQEXECPARAMS
JONJ_FUNCTION(pg_query_params);
#endif
#if HAVE_PQPREPARE
JONJ_FUNCTION(pg_prepare);
#endif
#if HAVE_PQEXECPREPARED
JONJ_FUNCTION(pg_execute);
#endif
JONJ_FUNCTION(pg_send_query);
#if HAVE_PQSENDQUERYPARAMS
JONJ_FUNCTION(pg_send_query_params);
#endif
#if HAVE_PQSENDPREPARE
JONJ_FUNCTION(pg_send_prepare);
#endif
#if HAVE_PQSENDQUERYPREPARED
JONJ_FUNCTION(pg_send_execute);
#endif
JONJ_FUNCTION(pg_cancel_query);
/* result functions */
JONJ_FUNCTION(pg_fetch_assoc);
JONJ_FUNCTION(pg_fetch_array);
JONJ_FUNCTION(pg_fetch_object);
JONJ_FUNCTION(pg_fetch_result);
JONJ_FUNCTION(pg_fetch_row);
JONJ_FUNCTION(pg_fetch_all);
JONJ_FUNCTION(pg_fetch_all_columns);
#if HAVE_PQCMDTUPLES
JONJ_FUNCTION(pg_affected_rows);
#endif
JONJ_FUNCTION(pg_get_result);
JONJ_FUNCTION(pg_result_seek);
JONJ_FUNCTION(pg_result_status);
JONJ_FUNCTION(pg_free_result);
JONJ_FUNCTION(pg_last_oid);
JONJ_FUNCTION(pg_num_rows);
JONJ_FUNCTION(pg_num_fields);
JONJ_FUNCTION(pg_field_name);
JONJ_FUNCTION(pg_field_num);
JONJ_FUNCTION(pg_field_size);
JONJ_FUNCTION(pg_field_type);
JONJ_FUNCTION(pg_field_type_oid);
JONJ_FUNCTION(pg_field_prtlen);
JONJ_FUNCTION(pg_field_is_null);
JONJ_FUNCTION(pg_field_table);
/* async message functions */
JONJ_FUNCTION(pg_get_notify);
JONJ_FUNCTION(pg_socket);
JONJ_FUNCTION(pg_consume_input);
JONJ_FUNCTION(pg_flush);
JONJ_FUNCTION(pg_get_pid);
/* error message functions */
JONJ_FUNCTION(pg_result_error);
#if HAVE_PQRESULTERRORFIELD
JONJ_FUNCTION(pg_result_error_field);
#endif
JONJ_FUNCTION(pg_last_error);
JONJ_FUNCTION(pg_last_notice);
/* copy functions */
JONJ_FUNCTION(pg_put_line);
JONJ_FUNCTION(pg_end_copy);
JONJ_FUNCTION(pg_copy_to);
JONJ_FUNCTION(pg_copy_from);
/* large object functions */
JONJ_FUNCTION(pg_lo_create);
JONJ_FUNCTION(pg_lo_unlink);
JONJ_FUNCTION(pg_lo_open);
JONJ_FUNCTION(pg_lo_close);
JONJ_FUNCTION(pg_lo_read);
JONJ_FUNCTION(pg_lo_write);
JONJ_FUNCTION(pg_lo_read_all);
JONJ_FUNCTION(pg_lo_import);
JONJ_FUNCTION(pg_lo_export);
JONJ_FUNCTION(pg_lo_seek);
JONJ_FUNCTION(pg_lo_tell);
#if HAVE_PG_LO_TRUNCATE
JONJ_FUNCTION(pg_lo_truncate);
#endif

/* debugging functions */
JONJ_FUNCTION(pg_trace);
JONJ_FUNCTION(pg_untrace);

/* utility functions */
JONJ_FUNCTION(pg_client_encoding);
JONJ_FUNCTION(pg_set_client_encoding);
#if HAVE_PQSETERRORVERBOSITY
JONJ_FUNCTION(pg_set_error_verbosity);
#endif
#if HAVE_PQESCAPE
JONJ_FUNCTION(pg_escape_string);
JONJ_FUNCTION(pg_escape_bytea);
JONJ_FUNCTION(pg_unescape_bytea);
JONJ_FUNCTION(pg_escape_literal);
JONJ_FUNCTION(pg_escape_identifier);
#endif

/* misc functions */
JONJ_FUNCTION(pg_meta_data);
JONJ_FUNCTION(pg_convert);
JONJ_FUNCTION(pg_insert);
JONJ_FUNCTION(pg_update);
JONJ_FUNCTION(pg_delete);
JONJ_FUNCTION(pg_select);

/* connection options - ToDo: Add async connection option */
#define PGSQL_CONNECT_FORCE_NEW     (1<<1)
#define PGSQL_CONNECT_ASYNC         (1<<2)
/* jonj_pgsql_convert options */
#define PGSQL_CONV_IGNORE_DEFAULT   (1<<1)     /* Do not use DEAFULT value by removing field from returned array */
#define PGSQL_CONV_FORCE_NULL       (1<<2)     /* Convert to NULL if string is null string */
#define PGSQL_CONV_IGNORE_NOT_NULL  (1<<3)     /* Ignore NOT NULL constraints */
#define PGSQL_CONV_OPTS             (PGSQL_CONV_IGNORE_DEFAULT|PGSQL_CONV_FORCE_NULL|PGSQL_CONV_IGNORE_NOT_NULL)
/* jonj_pgsql_insert/update/select/delete options */
#define PGSQL_DML_NO_CONV           (1<<8)     /* Do not call jonj_pgsql_convert() */
#define PGSQL_DML_EXEC              (1<<9)     /* Execute query */
#define PGSQL_DML_ASYNC             (1<<10)    /* Do async query */
#define PGSQL_DML_STRING            (1<<11)    /* Return query string */
#define PGSQL_DML_ESCAPE            (1<<12)    /* No convert, but escape only */


/* exported functions */
JONJ_PGSQL_API int jonj_pgsql_meta_data(PGconn *pg_link, const char *table_name, zval *meta, zend_bool extended TSRMLS_DC);
JONJ_PGSQL_API int jonj_pgsql_convert(PGconn *pg_link, const char *table_name, const zval *values, zval *result, ulong opt TSRMLS_DC);
JONJ_PGSQL_API int jonj_pgsql_insert(PGconn *pg_link, const char *table, zval *values, ulong opt, char **sql TSRMLS_DC);
JONJ_PGSQL_API int jonj_pgsql_update(PGconn *pg_link, const char *table, zval *values, zval *ids, ulong opt , char **sql TSRMLS_DC);
JONJ_PGSQL_API int jonj_pgsql_delete(PGconn *pg_link, const char *table, zval *ids, ulong opt, char **sql TSRMLS_DC);
JONJ_PGSQL_API int jonj_pgsql_select(PGconn *pg_link, const char *table, zval *ids, zval *ret_array, ulong opt, char **sql  TSRMLS_DC);
JONJ_PGSQL_API int jonj_pgsql_result2array(PGresult *pg_result, zval *ret_array TSRMLS_DC);

/* internal functions */
static void jonj_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent);
static void jonj_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void jonj_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list TSRMLS_DC);
static void jonj_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void jonj_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type);
static void jonj_pgsql_do_async(INTERNAL_FUNCTION_PARAMETERS,int entry_type);

static size_t jonj_pgsql_fd_write(jonj_stream *stream, const char *buf, size_t count TSRMLS_DC);
static size_t jonj_pgsql_fd_read(jonj_stream *stream, char *buf, size_t count TSRMLS_DC);
static int jonj_pgsql_fd_close(jonj_stream *stream, int close_handle TSRMLS_DC);
static int jonj_pgsql_fd_flush(jonj_stream *stream TSRMLS_DC);
static int jonj_pgsql_fd_set_option(jonj_stream *stream, int option, int value, void *ptrparam TSRMLS_DC);
static int jonj_pgsql_fd_cast(jonj_stream *stream, int cast_as, void **ret TSRMLS_DC);

typedef enum _jonj_pgsql_data_type {
	/* boolean */
	PG_BOOL,
	/* number */
	PG_OID,
	PG_INT2,
	PG_INT4,
	PG_INT8,
	PG_FLOAT4,
	PG_FLOAT8,
	PG_NUMERIC,
	PG_MONEY,
	/* character */
	PG_TEXT,
	PG_CHAR,
	PG_VARCHAR,
	/* time and interval */
	PG_UNIX_TIME,
	PG_UNIX_TIME_INTERVAL,
	PG_DATE,
	PG_TIME,
	PG_TIME_WITH_TIMEZONE,
	PG_TIMESTAMP,
	PG_TIMESTAMP_WITH_TIMEZONE,
	PG_INTERVAL,
	/* binary */
	PG_BYTEA,
	/* network */
	PG_CIDR,
	PG_INET,
	PG_MACADDR,
	/* bit */
	PG_BIT,
	PG_VARBIT,
	/* geometoric */
	PG_LINE,
	PG_LSEG,
	PG_POINT,
	PG_BOX,
	PG_PATH,
	PG_POLYGON,
	PG_CIRCLE,
	/* unknown and system */
	PG_UNKNOWN
} jonj_pgsql_data_type;

typedef struct pgLofp {
	PGconn *conn;
	int lofd;
} pgLofp;

typedef struct _jonj_pgsql_result_handle {
	PGconn *conn;
	PGresult *result;
	int row;
} pgsql_result_handle;

typedef struct _jonj_pgsql_notice {
	char *message;
	size_t len;
} jonj_pgsql_notice;

static jonj_stream_ops jonj_stream_pgsql_fd_ops = {
	jonj_pgsql_fd_write,
	jonj_pgsql_fd_read,
	jonj_pgsql_fd_close,
	jonj_pgsql_fd_flush,
	"PostgreSQL link",
	NULL, /* seek */
	jonj_pgsql_fd_cast, /* cast */
	NULL, /* stat */
	jonj_pgsql_fd_set_option
};

ZEND_BEGIN_MODULE_GLOBALS(pgsql)
	long default_link; /* default link when connection is omitted */
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	long auto_reset_persistent;
	int le_lofp,le_string;
	int ignore_notices,log_notices;
	HashTable notices;  /* notice message for each connection */
ZEND_END_MODULE_GLOBALS(pgsql)

ZEND_EXTERN_MODULE_GLOBALS(pgsql)

#ifdef ZTS
# define PGG(v) TSRMG(pgsql_globals_id, zend_pgsql_globals *, v)
#else
# define PGG(v) (pgsql_globals.v)
#endif

#endif

#else

#define pgsql_module_ptr NULL

#endif

#define jonjext_pgsql_ptr pgsql_module_ptr

#endif /* JONJ_PGSQL_H */
