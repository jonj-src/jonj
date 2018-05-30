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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef BASIC_FUNCTIONS_H
#define BASIC_FUNCTIONS_H

#include <sys/stat.h>

#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif

#include "jonj_filestat.h"

#include "zend_highlight.h"

#include "url_scanner_ex.h"

extern zend_module_entry basic_functions_module;
#define basic_functions_module_ptr &basic_functions_module

JONJ_MINIT_FUNCTION(basic);
JONJ_MSHUTDOWN_FUNCTION(basic);
JONJ_RINIT_FUNCTION(basic);
JONJ_RSHUTDOWN_FUNCTION(basic);
JONJ_MINFO_FUNCTION(basic);

JONJ_FUNCTION(constant);
JONJ_FUNCTION(sleep);
JONJ_FUNCTION(usleep);
#if HAVE_NANOSLEEP
JONJ_FUNCTION(time_nanosleep);
JONJ_FUNCTION(time_sleep_until);
#endif
JONJ_FUNCTION(flush);
#ifdef HAVE_INET_NTOP
JONJ_NAMED_FUNCTION(jonj_inet_ntop);
#endif
#ifdef HAVE_INET_PTON
JONJ_NAMED_FUNCTION(jonj_inet_pton);
#endif
JONJ_FUNCTION(ip2long);
JONJ_FUNCTION(long2ip);

/* system functions */
JONJ_FUNCTION(getenv);
JONJ_FUNCTION(putenv);

JONJ_FUNCTION(getopt);

JONJ_FUNCTION(get_current_user);
JONJ_FUNCTION(set_time_limit);

JONJ_FUNCTION(header_register_callback);

JONJ_FUNCTION(get_cfg_var);
JONJ_FUNCTION(set_magic_quotes_runtime);
JONJ_FUNCTION(get_magic_quotes_runtime);
JONJ_FUNCTION(get_magic_quotes_gpc);

JONJ_FUNCTION(error_log);
JONJ_FUNCTION(error_get_last);

JONJ_FUNCTION(call_user_func);
JONJ_FUNCTION(call_user_func_array);
JONJ_FUNCTION(call_user_method);
JONJ_FUNCTION(call_user_method_array);
JONJ_FUNCTION(forward_static_call);
JONJ_FUNCTION(forward_static_call_array);

JONJ_FUNCTION(register_shutdown_function);
JONJ_FUNCTION(highlight_file);
JONJ_FUNCTION(highlight_string);
JONJ_FUNCTION(jonj_strip_whitespace);
ZEND_API void jonj_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini);

JONJ_FUNCTION(ini_get);
JONJ_FUNCTION(ini_get_all);
JONJ_FUNCTION(ini_set);
JONJ_FUNCTION(ini_restore);
JONJ_FUNCTION(get_include_path);
JONJ_FUNCTION(set_include_path);
JONJ_FUNCTION(restore_include_path);

JONJ_FUNCTION(print_r);
JONJ_FUNCTION(fprintf);
JONJ_FUNCTION(vfprintf);

JONJ_FUNCTION(connection_aborted);
JONJ_FUNCTION(connection_status);
JONJ_FUNCTION(ignore_user_abort);

JONJ_FUNCTION(getservbyname);
JONJ_FUNCTION(getservbyport);
JONJ_FUNCTION(getprotobyname);
JONJ_FUNCTION(getprotobynumber);

JONJ_NAMED_FUNCTION(jonj_if_crc32);

JONJ_FUNCTION(register_tick_function);
JONJ_FUNCTION(unregister_tick_function);
#ifdef HAVE_GETLOADAVG
JONJ_FUNCTION(sys_getloadavg);
#endif

JONJ_FUNCTION(is_uploaded_file);
JONJ_FUNCTION(move_uploaded_file);

/* From the INI parser */
JONJ_FUNCTION(parse_ini_file);
JONJ_FUNCTION(parse_ini_string);
#if ZEND_DEBUG
JONJ_FUNCTION(config_get_hash);
#endif

JONJ_FUNCTION(str_rot13);
JONJ_FUNCTION(stream_get_filters);
JONJ_FUNCTION(stream_filter_register);
JONJ_FUNCTION(stream_bucket_make_writeable);
JONJ_FUNCTION(stream_bucket_prepend);
JONJ_FUNCTION(stream_bucket_append);
JONJ_FUNCTION(stream_bucket_new);
JONJ_MINIT_FUNCTION(user_filters);
JONJ_RSHUTDOWN_FUNCTION(user_filters);
JONJ_RSHUTDOWN_FUNCTION(browscap);

/* Left for BC (not binary safe!) */
JONJAPI int _jonj_error_log(int opt_err, char *message, char *opt, char *headers TSRMLS_DC);
JONJAPI int _jonj_error_log_ex(int opt_err, char *message, int message_len, char *opt, char *headers TSRMLS_DC);
JONJAPI int jonj_prefix_varname(zval *result, zval *prefix, char *var_name, int var_name_len, zend_bool add_underscore TSRMLS_DC);

#if SIZEOF_INT == 4
/* Most 32-bit and 64-bit systems have 32-bit ints */
typedef unsigned int jonj_uint32;
typedef signed int jonj_int32;
#elif SIZEOF_LONG == 4
/* 16-bit systems? */
typedef unsigned long jonj_uint32;
typedef signed long jonj_int32;
#else
#error Need type which holds 32 bits
#endif

#define MT_N (624)

typedef struct _jonj_basic_globals {
	HashTable *user_shutdown_function_names;
	HashTable putenv_ht;
	zval *strtok_zval;
	char *strtok_string;
	char *locale_string;
	char *strtok_last;
	char strtok_table[256];
	ulong strtok_len;
	char str_ebuf[40];
	zend_fcall_info array_walk_fci;
	zend_fcall_info_cache array_walk_fci_cache;
	zend_fcall_info user_compare_fci;
	zend_fcall_info_cache user_compare_fci_cache;
	zend_llist *user_tick_functions;

	zval *active_ini_file_section;
	
	/* pageinfo.c */
	long page_uid;
	long page_gid;
	long page_inode;
	time_t page_mtime;

	/* filestat.c && main/streams/streams.c */
	char *CurrentStatFile, *CurrentLStatFile;
	jonj_stream_statbuf ssb, lssb;

	/* rand.c */
	jonj_uint32   state[MT_N+1];  /* state vector + 1 extra to not violate ANSI C */
	jonj_uint32   *next;       /* next random value is computed from here */
	int      left;        /* can *next++ this many times before reloading */

	unsigned int rand_seed; /* Seed for rand(), in ts version */

	zend_bool rand_is_seeded; /* Whether rand() has been seeded */
	zend_bool mt_rand_is_seeded; /* Whether mt_rand() has been seeded */
    
	/* syslog.c */
	char *syslog_device;

	/* var.c */
	zend_class_entry *incomplete_class;
	unsigned serialize_lock; /* whether to use the locally supplied var_hash instead (__sleep/__wakeup) */
	struct {
		void *var_hash;
		unsigned level;
	} serialize;
	struct {
		void *var_hash;
		unsigned level;
	} unserialize;

	/* url_scanner_ex.re */
	url_adapt_state_ex_t url_adapt_state_ex;

#ifdef HAVE_MMAP
	void *mmap_file;
	size_t mmap_len;
#endif

	HashTable *user_filter_map;

	/* file.c */
#if defined(_REENTRANT) && defined(HAVE_MBRLEN) && defined(HAVE_MBSTATE_T)
	mbstate_t mblen_state;
#endif

	int umask;
} jonj_basic_globals;

#ifdef ZTS
#define BG(v) TSRMG(basic_globals_id, jonj_basic_globals *, v)
JONJAPI extern int basic_globals_id;
#else
#define BG(v) (basic_globals.v)
JONJAPI extern jonj_basic_globals basic_globals;
#endif

#if HAVE_PUTENV
typedef struct {
	char *putenv_string;
	char *previous_value;
	char *key;
	int key_len;
} putenv_entry;
#endif

JONJAPI double jonj_get_nan(void);
JONJAPI double jonj_get_inf(void);

typedef struct _jonj_shutdown_function_entry {
	zval **arguments;
	int arg_count;
} jonj_shutdown_function_entry;

JONJAPI extern zend_bool register_user_shutdown_function(char *function_name, size_t function_len, jonj_shutdown_function_entry *shutdown_function_entry TSRMLS_DC);
JONJAPI extern zend_bool remove_user_shutdown_function(char *function_name, size_t function_len TSRMLS_DC);
JONJAPI extern zend_bool append_user_shutdown_function(jonj_shutdown_function_entry shutdown_function_entry TSRMLS_DC);

JONJAPI void jonj_call_shutdown_functions(TSRMLS_D);
JONJAPI void jonj_free_shutdown_functions(TSRMLS_D);


#endif /* BASIC_FUNCTIONS_H */
