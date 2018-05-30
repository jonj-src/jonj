/*
  +----------------------------------------------------------------------+
  | JONJ Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the JONJ license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_01.txt                                  |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "jonj.h"
#include "jonj_ini.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_reverse_api.h"
#include "ext/standard/info.h"
#include "ext/standard/jonj_smart_str.h"

/* {{{ mysqlnd_functions[]
 *
 * Every user visible function must have an entry in mysqlnd_functions[].
 */
static zend_function_entry mysqlnd_functions[] = {
	JONJ_FE_END
};
/* }}} */


/* {{{ mysqlnd_minfo_print_hash */
JONJAPI void
mysqlnd_minfo_print_hash(zval *values)
{
	zval **values_entry;
	HashPosition pos_values;

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(values), &pos_values);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(values), (void **)&values_entry, &pos_values) == SUCCESS) {
		char	*string_key;
		uint	string_key_len;
		ulong	num_key;

		zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &string_key, &string_key_len, &num_key, 0, &pos_values);

		convert_to_string(*values_entry);
		jonj_info_print_table_row(2, string_key, Z_STRVAL_PP(values_entry));

		zend_hash_move_forward_ex(Z_ARRVAL_P(values), &pos_values);
	}
}
/* }}} */


/* {{{ mysqlnd_minfo_dump_plugin_stats */
static int
mysqlnd_minfo_dump_plugin_stats(void *pDest, void * argument TSRMLS_DC)
{
	struct st_mysqlnd_plugin_header * plugin_header = *(struct st_mysqlnd_plugin_header **) pDest;
	if (plugin_header->plugin_stats.values) {
		char buf[64];
		zval values;
		snprintf(buf, sizeof(buf), "%s statistics", plugin_header->plugin_name);

		mysqlnd_fill_stats_hash(plugin_header->plugin_stats.values, plugin_header->plugin_stats.names, &values TSRMLS_CC ZEND_FILE_LINE_CC); 

		jonj_info_print_table_start();
		jonj_info_print_table_header(2, buf, "");
		mysqlnd_minfo_print_hash(&values);
		jonj_info_print_table_end();
		zval_dtor(&values);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */


/* {{{ mysqlnd_minfo_dump_loaded_plugins */
static int 
mysqlnd_minfo_dump_loaded_plugins(void *pDest, void * buf TSRMLS_DC)
{
	smart_str * buffer = (smart_str *) buf;
	struct st_mysqlnd_plugin_header * plugin_header = *(struct st_mysqlnd_plugin_header **) pDest;
	if (plugin_header->plugin_name) {
		if (buffer->len) {
			smart_str_appendc(buffer, ',');
		}
		smart_str_appends(buffer, plugin_header->plugin_name);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ mysqlnd_minfo_dump_api_plugins */
static void
mysqlnd_minfo_dump_api_plugins(smart_str * buffer TSRMLS_DC)
{
	HashTable *ht = mysqlnd_reverse_api_get_api_list(TSRMLS_C);
	HashPosition pos;
	MYSQLND_REVERSE_API **ext;

	for (zend_hash_internal_pointer_reset_ex(ht, &pos);
	     zend_hash_get_current_data_ex(ht, (void **) &ext, &pos) == SUCCESS;
	     zend_hash_move_forward_ex(ht, &pos)
	) {
		if (buffer->len) {
			smart_str_appendc(buffer, ',');
		}
		smart_str_appends(buffer, (*ext)->module->name);
	}
}
/* }}} */


/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(mysqlnd)
{
	char buf[32];

	jonj_info_print_table_start();
	jonj_info_print_table_header(2, "mysqlnd", "enabled");
	jonj_info_print_table_row(2, "Version", mysqlnd_get_client_info());
	jonj_info_print_table_row(2, "Compression",
#ifdef MYSQLND_COMPRESSION_ENABLED
								"supported");
#else
								"not supported");
#endif
	jonj_info_print_table_row(2, "core SSL",
#ifdef MYSQLND_SSL_SUPPORTED
								"supported");
#else
								"not supported");
#endif
	jonj_info_print_table_row(2, "extended SSL",
#ifdef MYSQLND_HAVE_SSL
								"supported");
#else
								"not supported");
#endif
	snprintf(buf, sizeof(buf), "%ld", MYSQLND_G(net_cmd_buffer_size));
	jonj_info_print_table_row(2, "Command buffer size", buf);
	snprintf(buf, sizeof(buf), "%ld", MYSQLND_G(net_read_buffer_size));
	jonj_info_print_table_row(2, "Read buffer size", buf);
	snprintf(buf, sizeof(buf), "%ld", MYSQLND_G(net_read_timeout));
	jonj_info_print_table_row(2, "Read timeout", buf);
	jonj_info_print_table_row(2, "Collecting statistics", MYSQLND_G(collect_statistics)? "Yes":"No");
	jonj_info_print_table_row(2, "Collecting memory statistics", MYSQLND_G(collect_memory_statistics)? "Yes":"No");

	jonj_info_print_table_row(2, "Tracing", MYSQLND_G(debug)? MYSQLND_G(debug):"n/a");

	/* loaded plugins */
	{
		smart_str tmp_str = {0, 0, 0};
		mysqlnd_plugin_apply_with_argument(mysqlnd_minfo_dump_loaded_plugins, &tmp_str);
		smart_str_0(&tmp_str);
		jonj_info_print_table_row(2, "Loaded plugins", tmp_str.c);
		smart_str_free(&tmp_str);

		mysqlnd_minfo_dump_api_plugins(&tmp_str TSRMLS_CC);
		smart_str_0(&tmp_str);
		jonj_info_print_table_row(2, "API Extensions", tmp_str.c);
		smart_str_free(&tmp_str);
	}

	jonj_info_print_table_end();


	/* Print client stats */
	mysqlnd_plugin_apply_with_argument(mysqlnd_minfo_dump_plugin_stats, NULL);
}
/* }}} */


JONJAPI ZEND_DECLARE_MODULE_GLOBALS(mysqlnd)


/* {{{ JONJ_GINIT_FUNCTION
 */
static JONJ_GINIT_FUNCTION(mysqlnd)
{
	mysqlnd_globals->collect_statistics = TRUE;
	mysqlnd_globals->collect_memory_statistics = FALSE;
	mysqlnd_globals->debug = NULL;	/* The actual string */
	mysqlnd_globals->dbg = NULL;	/* The DBG object*/
	mysqlnd_globals->trace_alloc_settings = NULL;
	mysqlnd_globals->trace_alloc = NULL;
	mysqlnd_globals->net_cmd_buffer_size = MYSQLND_NET_CMD_BUFFER_MIN_SIZE;
	mysqlnd_globals->net_read_buffer_size = 32768;
	mysqlnd_globals->net_read_timeout = 31536000;
	mysqlnd_globals->log_mask = 0;
	mysqlnd_globals->mempool_default_size = 16000;
	mysqlnd_globals->debug_emalloc_fail_threshold = -1;
	mysqlnd_globals->debug_ecalloc_fail_threshold = -1;
	mysqlnd_globals->debug_erealloc_fail_threshold = -1;
	mysqlnd_globals->debug_malloc_fail_threshold = -1;
	mysqlnd_globals->debug_calloc_fail_threshold = -1;
	mysqlnd_globals->debug_realloc_fail_threshold = -1;
	mysqlnd_globals->sha256_server_public_key = NULL;
	mysqlnd_globals->fetch_data_copy = FALSE;
}
/* }}} */


static JONJ_INI_MH(OnUpdateNetCmdBufferSize)
{
	long long_value = atol(new_value);
	if (long_value < MYSQLND_NET_CMD_BUFFER_MIN_SIZE) {
		return FAILURE;
	}
	MYSQLND_G(net_cmd_buffer_size) = long_value;

	return SUCCESS;
}

/* {{{ JONJ_INI_BEGIN
*/
JONJ_INI_BEGIN()
	STD_JONJ_INI_BOOLEAN("mysqlnd.collect_statistics",	"1", 	JONJ_INI_ALL,	OnUpdateBool,	collect_statistics, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_BOOLEAN("mysqlnd.collect_memory_statistics","0",JONJ_INI_SYSTEM, OnUpdateBool,	collect_memory_statistics, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.debug",					NULL, 	JONJ_INI_SYSTEM, OnUpdateString,	debug, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.trace_alloc",			NULL, 	JONJ_INI_SYSTEM, OnUpdateString,	trace_alloc_settings, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.net_cmd_buffer_size",	MYSQLND_NET_CMD_BUFFER_MIN_SIZE_STR,	JONJ_INI_ALL,	OnUpdateNetCmdBufferSize,	net_cmd_buffer_size,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.net_read_buffer_size",	"32768",JONJ_INI_ALL,	OnUpdateLong,	net_read_buffer_size,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.net_read_timeout",	"31536000",	JONJ_INI_SYSTEM, OnUpdateLong,	net_read_timeout, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.log_mask",				"0", 	JONJ_INI_ALL,	OnUpdateLong,	log_mask, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.mempool_default_size","16000",   JONJ_INI_ALL,	OnUpdateLong,	mempool_default_size,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.sha256_server_public_key",NULL, 	JONJ_INI_PERDIR, OnUpdateString,	sha256_server_public_key, zend_mysqlnd_globals, mysqlnd_globals)
	STD_JONJ_INI_BOOLEAN("mysqlnd.fetch_data_copy",	"0", 		JONJ_INI_ALL,	OnUpdateBool,	fetch_data_copy, zend_mysqlnd_globals, mysqlnd_globals)
#if JONJ_DEBUG
	STD_JONJ_INI_ENTRY("mysqlnd.debug_emalloc_fail_threshold","-1",   JONJ_INI_SYSTEM,	OnUpdateLong,	debug_emalloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.debug_ecalloc_fail_threshold","-1",   JONJ_INI_SYSTEM,	OnUpdateLong,	debug_ecalloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.debug_erealloc_fail_threshold","-1",   JONJ_INI_SYSTEM,	OnUpdateLong,	debug_erealloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)

	STD_JONJ_INI_ENTRY("mysqlnd.debug_malloc_fail_threshold","-1",   JONJ_INI_SYSTEM,	OnUpdateLong,	debug_malloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.debug_calloc_fail_threshold","-1",   JONJ_INI_SYSTEM,	OnUpdateLong,	debug_calloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
	STD_JONJ_INI_ENTRY("mysqlnd.debug_realloc_fail_threshold","-1",   JONJ_INI_SYSTEM,	OnUpdateLong,	debug_realloc_fail_threshold,	zend_mysqlnd_globals,		mysqlnd_globals)
#endif
JONJ_INI_END()
/* }}} */


/* {{{ JONJ_MINIT_FUNCTION
 */
static JONJ_MINIT_FUNCTION(mysqlnd)
{
	REGISTER_INI_ENTRIES();

	mysqlnd_library_init(TSRMLS_C);
	return SUCCESS;
}
/* }}} */


/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
static JONJ_MSHUTDOWN_FUNCTION(mysqlnd)
{
	mysqlnd_library_end(TSRMLS_C);

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */


#if JONJ_DEBUG
/* {{{ JONJ_RINIT_FUNCTION
 */
static JONJ_RINIT_FUNCTION(mysqlnd)
{
	if (MYSQLND_G(debug)) {
		struct st_mysqlnd_plugin_trace_log * trace_log_plugin = mysqlnd_plugin_find("debug_trace");
		MYSQLND_G(dbg) = NULL;
		if (trace_log_plugin) {
			MYSQLND_DEBUG * dbg = trace_log_plugin->methods.trace_instance_init(mysqlnd_debug_std_no_trace_funcs TSRMLS_CC);
			MYSQLND_DEBUG * trace_alloc = trace_log_plugin->methods.trace_instance_init(NULL TSRMLS_CC);
			if (!dbg || !trace_alloc) {
				return FAILURE;
			}
			dbg->m->set_mode(dbg, MYSQLND_G(debug));
			trace_alloc->m->set_mode(trace_alloc, MYSQLND_G(trace_alloc_settings));
			MYSQLND_G(dbg) = dbg;
			MYSQLND_G(trace_alloc) = trace_alloc;
		}
	}
	return SUCCESS;
}
/* }}} */
#endif


#if JONJ_DEBUG
/* {{{ JONJ_RSHUTDOWN_FUNCTION
 */
static JONJ_RSHUTDOWN_FUNCTION(mysqlnd)
{
	MYSQLND_DEBUG * dbg = MYSQLND_G(dbg);
	MYSQLND_DEBUG * trace_alloc = MYSQLND_G(trace_alloc);
	DBG_ENTER("RSHUTDOWN");
	if (dbg) {
		dbg->m->close(dbg);
		dbg->m->free_handle(dbg);
		MYSQLND_G(dbg) = NULL;
	}
	if (trace_alloc) {
		trace_alloc->m->close(trace_alloc);
		trace_alloc->m->free_handle(trace_alloc);
		MYSQLND_G(trace_alloc) = NULL;
	}
	return SUCCESS;
}
/* }}} */
#endif



static const zend_module_dep mysqlnd_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_END
};

/* {{{ mysqlnd_module_entry
 */
zend_module_entry mysqlnd_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	mysqlnd_deps,
	"mysqlnd",
	mysqlnd_functions,
	JONJ_MINIT(mysqlnd),
	JONJ_MSHUTDOWN(mysqlnd),
#if JONJ_DEBUG
	JONJ_RINIT(mysqlnd),
#else
	NULL,
#endif
#if JONJ_DEBUG
	JONJ_RSHUTDOWN(mysqlnd),
#else
	NULL,
#endif
	JONJ_MINFO(mysqlnd),
	MYSQLND_VERSION,
	JONJ_MODULE_GLOBALS(mysqlnd),
	JONJ_GINIT(mysqlnd),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/* {{{ COMPILE_DL_MYSQLND */
#ifdef COMPILE_DL_MYSQLND
ZEND_GET_MODULE(mysqlnd)
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
