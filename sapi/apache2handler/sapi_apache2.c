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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Parts based on Apache 1.3 SAPI module by                    |
   |          Rasmus Lerdorf and Zeev Suraski                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "jonj.h"
#include "jonj_main.h"
#include "jonj_ini.h"
#include "jonj_variables.h"
#include "SAPI.h"

#include <fcntl.h>

#include "ext/standard/jonj_smart_str.h"
#ifndef NETWARE
#include "ext/standard/jonj_standard.h"
#else
#include "ext/standard/basic_functions.h"
#endif

#include "apr_strings.h"
#include "ap_config.h"
#include "util_filter.h"
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_log.h"
#include "http_main.h"
#include "util_script.h"
#include "http_core.h"
#include "ap_mpm.h"

#include "jonj_apache.h"

#ifdef JONJ_WIN32
# if _MSC_VER <= 1300
#  include "win32/jonj_strtoi64.h"
# endif
#endif

/* UnixWare and Netware define shutdown to _shutdown, which causes problems later
 * on when using a structure member named shutdown. Since this source
 * file does not use the system call shutdown, it is safe to #undef it.K
 */
#undef shutdown

#define JONJ_MAGIC_TYPE "application/x-httpd-jonj"
#define JONJ_SOURCE_MAGIC_TYPE "application/x-httpd-jonj-source"
#define JONJ_SCRIPT "jonj5-script"

/* A way to specify the location of the jonj.ini dir in an apache directive */
char *apache2_jonj_ini_path_override = NULL;

static int
jonj_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	request_rec *r;
	jonj_struct *ctx;

	ctx = SG(server_context);
	r = ctx->r;

	if (ap_rwrite(str, str_length, r) < 0) {
		jonj_handle_aborted_connection();
	}

	return str_length; /* we always consume all the data passed to us. */
}

static int
jonj_apache_sapi_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	jonj_struct *ctx;
	char *val, *ptr;

	ctx = SG(server_context);

	switch (op) {
		case SAPI_HEADER_DELETE:
			apr_table_unset(ctx->r->headers_out, sapi_header->header);
			return 0;

		case SAPI_HEADER_DELETE_ALL:
			apr_table_clear(ctx->r->headers_out);
			return 0;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
			val = strchr(sapi_header->header, ':');

			if (!val) {
				return 0;
			}
			ptr = val;

			*val = '\0';

			do {
				val++;
			} while (*val == ' ');

			if (!strcasecmp(sapi_header->header, "content-type")) {
				if (ctx->content_type) {
					efree(ctx->content_type);
				}
				ctx->content_type = estrdup(val);
			} else if (!strcasecmp(sapi_header->header, "content-length")) {
				apr_off_t clen = 0;

				if (APR_SUCCESS != apr_strtoff(&clen, val, (char **) NULL, 10)) {
					/* We'll fall back to strtol, since that's what we used to
					 * do anyway. */
					clen = (apr_off_t) strtol(val, (char **) NULL, 10);
				}

				ap_set_content_length(ctx->r, clen);
			} else if (op == SAPI_HEADER_REPLACE) {
				apr_table_set(ctx->r->headers_out, sapi_header->header, val);
			} else {
				apr_table_add(ctx->r->headers_out, sapi_header->header, val);
			}

			*ptr = ':';

			return SAPI_HEADER_ADD;

		default:
			return 0;
	}
}

static int
jonj_apache_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	jonj_struct *ctx = SG(server_context);
	const char *sline = SG(sapi_headers).http_status_line;

	ctx->r->status = SG(sapi_headers).http_response_code;

	/* httpd requires that r->status_line is set to the first digit of
	 * the status-code: */
	if (sline && strlen(sline) > 12 && strncmp(sline, "HTTP/1.", 7) == 0 && sline[8] == ' ') {
		ctx->r->status_line = apr_pstrdup(ctx->r->pool, sline + 9);
		ctx->r->proto_num = 1000 + (sline[7]-'0');
		if ((sline[7]-'0') == 0) {
			apr_table_set(ctx->r->subprocess_env, "force-response-1.0", "true");
		}
	}

	/*	call ap_set_content_type only once, else each time we call it,
		configured output filters for that content type will be added */
	if (!ctx->content_type) {
		ctx->content_type = sapi_get_default_content_type(TSRMLS_C);
	}
	ap_set_content_type(ctx->r, apr_pstrdup(ctx->r->pool, ctx->content_type));
	efree(ctx->content_type);
	ctx->content_type = NULL;

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
jonj_apache_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
	apr_size_t len, tlen=0;
	jonj_struct *ctx = SG(server_context);
	request_rec *r;
	apr_bucket_brigade *brigade;

	r = ctx->r;
	brigade = ctx->brigade;
	len = count_bytes;

	/*
	 * This loop is needed because ap_get_brigade() can return us partial data
	 * which would cause premature termination of request read. Therefor we
	 * need to make sure that if data is available we fill the buffer completely.
	 */

	while (ap_get_brigade(r->input_filters, brigade, AP_MODE_READBYTES, APR_BLOCK_READ, len) == APR_SUCCESS) {
		apr_brigade_flatten(brigade, buf, &len);
		apr_brigade_cleanup(brigade);
		tlen += len;
		if (tlen == count_bytes || !len) {
			break;
		}
		buf += len;
		len = count_bytes - tlen;
	}

	return tlen;
}

static struct stat*
jonj_apache_sapi_get_stat(TSRMLS_D)
{
	jonj_struct *ctx = SG(server_context);

	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
	ctx->finfo.st_dev = ctx->r->finfo.device;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
#if defined(NETWARE) && defined(CLIB_STAT_PATCH)
	ctx->finfo.st_atime.tv_sec = apr_time_sec(ctx->r->finfo.atime);
	ctx->finfo.st_mtime.tv_sec = apr_time_sec(ctx->r->finfo.mtime);
	ctx->finfo.st_ctime.tv_sec = apr_time_sec(ctx->r->finfo.ctime);
#else
	ctx->finfo.st_atime = apr_time_sec(ctx->r->finfo.atime);
	ctx->finfo.st_mtime = apr_time_sec(ctx->r->finfo.mtime);
	ctx->finfo.st_ctime = apr_time_sec(ctx->r->finfo.ctime);
#endif

	ctx->finfo.st_size = ctx->r->finfo.size;
	ctx->finfo.st_nlink = ctx->r->finfo.nlink;

	return &ctx->finfo;
}

static char *
jonj_apache_sapi_read_cookies(TSRMLS_D)
{
	jonj_struct *ctx = SG(server_context);
	const char *http_cookie;

	http_cookie = apr_table_get(ctx->r->headers_in, "cookie");

	/* The SAPI interface should use 'const char *' */
	return (char *) http_cookie;
}

static char *
jonj_apache_sapi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	jonj_struct *ctx = SG(server_context);
	const char *env_var;

	if (ctx == NULL) {
		return NULL;
	}

	env_var = apr_table_get(ctx->r->subprocess_env, name);

	return (char *) env_var;
}

static void
jonj_apache_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	jonj_struct *ctx = SG(server_context);
	const apr_array_header_t *arr = apr_table_elts(ctx->r->subprocess_env);
	char *key, *val;
	int new_val_len;

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) {
			val = "";
		}
		if (sapi_module.input_filter(PARSE_SERVER, key, &val, strlen(val), (unsigned int *)&new_val_len TSRMLS_CC)) {
			jonj_register_variable_safe(key, val, new_val_len, track_vars_array TSRMLS_CC);
		}
	APR_ARRAY_FOREACH_CLOSE()

	if (sapi_module.input_filter(PARSE_SERVER, "JONJ_SELF", &ctx->r->uri, strlen(ctx->r->uri), (unsigned int *)&new_val_len TSRMLS_CC)) {
		jonj_register_variable_safe("JONJ_SELF", ctx->r->uri, new_val_len, track_vars_array TSRMLS_CC);
	}
}

static void
jonj_apache_sapi_flush(void *server_context)
{
	jonj_struct *ctx;
	request_rec *r;
	TSRMLS_FETCH();

	ctx = server_context;

	/* If we haven't registered a server_context yet,
	 * then don't bother flushing. */
	if (!server_context) {
		return;
	}

	r = ctx->r;

	sapi_send_headers(TSRMLS_C);

	r->status = SG(sapi_headers).http_response_code;
	SG(headers_sent) = 1;

	if (ap_rflush(r) < 0 || r->connection->aborted) {
		jonj_handle_aborted_connection();
	}
}

static void jonj_apache_sapi_log_message(char *msg TSRMLS_DC)
{
	jonj_struct *ctx;

	ctx = SG(server_context);

	if (ctx == NULL) { /* we haven't initialized our ctx yet, oh well */
		ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_STARTUP, 0, NULL, "%s", msg);
	} else {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, ctx->r, "%s", msg);
	}
}

static void jonj_apache_sapi_log_message_ex(char *msg, request_rec *r TSRMLS_DC)
{
	if (r) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, msg, r->filename);
	} else {
		jonj_apache_sapi_log_message(msg TSRMLS_CC);
	}
}

static double jonj_apache_sapi_get_request_time(TSRMLS_D)
{
	jonj_struct *ctx = SG(server_context);
	return ((double) apr_time_as_msec(ctx->r->request_time)) / 1000.0;
}

extern zend_module_entry jonj_apache_module;

static int jonj_apache2_startup(sapi_module_struct *sapi_module)
{
	if (jonj_module_startup(sapi_module, &jonj_apache_module, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

static sapi_module_struct apache2_sapi_module = {
	"apache2handler",
	"Apache 2.0 Handler",

	jonj_apache2_startup,				/* startup */
	jonj_module_shutdown_wrapper,			/* shutdown */

	NULL,						/* activate */
	NULL,						/* deactivate */

	jonj_apache_sapi_ub_write,			/* unbuffered write */
	jonj_apache_sapi_flush,				/* flush */
	jonj_apache_sapi_get_stat,			/* get uid */
	jonj_apache_sapi_getenv,				/* getenv */

	jonj_error,					/* error handler */

	jonj_apache_sapi_header_handler,			/* header handler */
	jonj_apache_sapi_send_headers,			/* send headers handler */
	NULL,						/* send header handler */

	jonj_apache_sapi_read_post,			/* read POST data */
	jonj_apache_sapi_read_cookies,			/* read Cookies */

	jonj_apache_sapi_register_variables,
	jonj_apache_sapi_log_message,			/* Log message */
	jonj_apache_sapi_get_request_time,		/* Request Time */
	NULL,						/* Child Terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static apr_status_t jonj_apache_server_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif
	return APR_SUCCESS;
}

static apr_status_t jonj_apache_child_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
#if defined(ZTS) && !defined(JONJ_WIN32)
	tsrm_shutdown();
#endif
	return APR_SUCCESS;
}

static void jonj_apache_add_version(apr_pool_t *p)
{
	TSRMLS_FETCH();
	if (PG(expose_jonj)) {
		ap_add_version_component(p, "JONJ/" JONJ_VERSION);
	}
}

static int jonj_pre_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp)
{
#ifndef ZTS
	int threaded_mpm;

	ap_mpm_query(AP_MPMQ_IS_THREADED, &threaded_mpm);
	if(threaded_mpm) {
		ap_log_error(APLOG_MARK, APLOG_CRIT, 0, 0, "Apache is running a threaded MPM, but your JONJ Module is not compiled to be threadsafe.  You need to recompile JONJ.");
		return DONE;
	}
#endif
	/* When this is NULL, apache won't override the hard-coded default
	 * jonj.ini path setting. */
	apache2_jonj_ini_path_override = NULL;
	return OK;
}

static int
jonj_apache_server_startup(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
	void *data = NULL;
	const char *userdata_key = "apache2hook_post_config";

	/* Apache will load, unload and then reload a DSO module. This
	 * prevents us from starting JONJ until the second load. */
	apr_pool_userdata_get(&data, userdata_key, s->process->pool);
	if (data == NULL) {
		/* We must use set() here and *not* setn(), otherwise the
		 * static string pointed to by userdata_key will be mapped
		 * to a different location when the DSO is reloaded and the
		 * pointers won't match, causing get() to return NULL when
		 * we expected it to return non-NULL. */
		apr_pool_userdata_set((const void *)1, userdata_key, apr_pool_cleanup_null, s->process->pool);
		return OK;
	}

	/* Set up our overridden path. */
	if (apache2_jonj_ini_path_override) {
		apache2_sapi_module.jonj_ini_path_override = apache2_jonj_ini_path_override;
	}
#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
#endif
	sapi_startup(&apache2_sapi_module);
	apache2_sapi_module.startup(&apache2_sapi_module);
	apr_pool_cleanup_register(pconf, NULL, jonj_apache_server_shutdown, apr_pool_cleanup_null);
	jonj_apache_add_version(pconf);

	return OK;
}

static apr_status_t jonj_server_context_cleanup(void *data_)
{
	void **data = data_;
	*data = NULL;
	return APR_SUCCESS;
}

static int jonj_apache_request_ctor(request_rec *r, jonj_struct *ctx TSRMLS_DC)
{
	char *content_length;
	const char *auth;

	SG(sapi_headers).http_response_code = !r->status ? HTTP_OK : r->status;
	SG(request_info).content_type = apr_table_get(r->headers_in, "Content-Type");
	SG(request_info).query_string = apr_pstrdup(r->pool, r->args);
	SG(request_info).request_method = r->method;
	SG(request_info).proto_num = r->proto_num;
	SG(request_info).request_uri = apr_pstrdup(r->pool, r->uri);
	SG(request_info).path_translated = apr_pstrdup(r->pool, r->filename);
	r->no_local_copy = 1;

	content_length = (char *) apr_table_get(r->headers_in, "Content-Length");
	SG(request_info).content_length = (content_length ? atol(content_length) : 0);

	apr_table_unset(r->headers_out, "Content-Length");
	apr_table_unset(r->headers_out, "Last-Modified");
	apr_table_unset(r->headers_out, "Expires");
	apr_table_unset(r->headers_out, "ETag");

	auth = apr_table_get(r->headers_in, "Authorization");
	jonj_handle_auth_data(auth TSRMLS_CC);

	if (SG(request_info).auth_user == NULL && r->user) {
		SG(request_info).auth_user = estrdup(r->user);
	}

	ctx->r->user = apr_pstrdup(ctx->r->pool, SG(request_info).auth_user);

	return jonj_request_startup(TSRMLS_C);
}

static void jonj_apache_request_dtor(request_rec *r TSRMLS_DC)
{
	jonj_request_shutdown(NULL);
}

static void jonj_apache_ini_dtor(request_rec *r, request_rec *p TSRMLS_DC)
{
	if (strcmp(r->protocol, "INCLUDED")) {
		zend_try { zend_ini_deactivate(TSRMLS_C); } zend_end_try();
	} else {
typedef struct {
	HashTable config;
} jonj_conf_rec;
		char *str;
		uint str_len;
		jonj_conf_rec *c = ap_get_module_config(r->per_dir_config, &jonj5_module);

		for (zend_hash_internal_pointer_reset(&c->config);
			zend_hash_get_current_key_ex(&c->config, &str, &str_len, NULL, 0,  NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&c->config)
		) {
			zend_restore_ini_entry(str, str_len, ZEND_INI_STAGE_SHUTDOWN);
		}
	}
	if (p) {
		((jonj_struct *)SG(server_context))->r = p;
	} else {
		apr_pool_cleanup_run(r->pool, (void *)&SG(server_context), jonj_server_context_cleanup);
	}
}

static int jonj_handler(request_rec *r)
{
	jonj_struct * volatile ctx;
	void *conf;
	apr_bucket_brigade * volatile brigade;
	apr_bucket *bucket;
	apr_status_t rv;
	request_rec * volatile parent_req = NULL;
	TSRMLS_FETCH();

#define JONJAP_INI_OFF jonj_apache_ini_dtor(r, parent_req TSRMLS_CC);

	conf = ap_get_module_config(r->per_dir_config, &jonj5_module);

	/* apply_config() needs r in some cases, so allocate server_context early */
	ctx = SG(server_context);
	if (ctx == NULL || (ctx && ctx->request_processed && !strcmp(r->protocol, "INCLUDED"))) {
normal:
		ctx = SG(server_context) = apr_pcalloc(r->pool, sizeof(*ctx));
		/* register a cleanup so we clear out the SG(server_context)
		 * after each request. Note: We pass in the pointer to the
		 * server_context in case this is handled by a different thread.
		 */
		apr_pool_cleanup_register(r->pool, (void *)&SG(server_context), jonj_server_context_cleanup, apr_pool_cleanup_null);
		ctx->r = r;
		ctx = NULL; /* May look weird to null it here, but it is to catch the right case in the first_try later on */
	} else {
		parent_req = ctx->r;
		ctx->r = r;
	}
	apply_config(conf);

	if (strcmp(r->handler, JONJ_MAGIC_TYPE) && strcmp(r->handler, JONJ_SOURCE_MAGIC_TYPE) && strcmp(r->handler, JONJ_SCRIPT)) {
		/* Check for xbithack in this case. */
		if (!AP2(xbithack) || strcmp(r->handler, "text/html") || !(r->finfo.protection & APR_UEXECUTE)) {
			JONJAP_INI_OFF;
			return DECLINED;
		}
	}

	/* Give a 404 if PATH_INFO is used but is explicitly disabled in
	 * the configuration; default behaviour is to accept. */
	if (r->used_path_info == AP_REQ_REJECT_PATH_INFO
		&& r->path_info && r->path_info[0]) {
		JONJAP_INI_OFF;
		return HTTP_NOT_FOUND;
	}

	/* handle situations where user turns the engine off */
	if (!AP2(engine)) {
		JONJAP_INI_OFF;
		return DECLINED;
	}

	if (r->finfo.filetype == 0) {
		jonj_apache_sapi_log_message_ex("script '%s' not found or unable to stat", r TSRMLS_CC);
		JONJAP_INI_OFF;
		return HTTP_NOT_FOUND;
	}
	if (r->finfo.filetype == APR_DIR) {
		jonj_apache_sapi_log_message_ex("attempt to invoke directory '%s' as script", r TSRMLS_CC);
		JONJAP_INI_OFF;
		return HTTP_FORBIDDEN;
	}

	/* Setup the CGI variables if this is the main request */
	if (r->main == NULL ||
		/* .. or if the sub-request environment differs from the main-request. */
		r->subprocess_env != r->main->subprocess_env
	) {
		/* setup standard CGI variables */
		ap_add_common_vars(r);
		ap_add_cgi_vars(r);
	}

zend_first_try {

	if (ctx == NULL) {
		brigade = apr_brigade_create(r->pool, r->connection->bucket_alloc);
		ctx = SG(server_context);
		ctx->brigade = brigade;

		if (jonj_apache_request_ctor(r, ctx TSRMLS_CC)!=SUCCESS) {
			zend_bailout();
		}
	} else {
		if (!parent_req) {
			parent_req = ctx->r;
		}
		if (parent_req && parent_req->handler &&
				strcmp(parent_req->handler, JONJ_MAGIC_TYPE) &&
				strcmp(parent_req->handler, JONJ_SOURCE_MAGIC_TYPE) &&
				strcmp(parent_req->handler, JONJ_SCRIPT)) {
			if (jonj_apache_request_ctor(r, ctx TSRMLS_CC)!=SUCCESS) {
				zend_bailout();
			}
		}

		/*
		 * check if coming due to ErrorDocument
		 * We make a special exception of 413 (Invalid POST request) as the invalidity of the request occurs
		 * during processing of the request by JONJ during POST processing. Therefor we need to re-use the exiting
		 * JONJ instance to handle the request rather then creating a new one.
		*/
		if (parent_req && parent_req->status != HTTP_OK && parent_req->status != 413 && strcmp(r->protocol, "INCLUDED")) {
			parent_req = NULL;
			goto normal;
		}
		ctx->r = r;
		brigade = ctx->brigade;
	}

	if (AP2(last_modified)) {
		ap_update_mtime(r, r->finfo.mtime);
		ap_set_last_modified(r);
	}

	/* Determine if we need to parse the file or show the source */
	if (strncmp(r->handler, JONJ_SOURCE_MAGIC_TYPE, sizeof(JONJ_SOURCE_MAGIC_TYPE) - 1) == 0) {
		zend_syntax_highlighter_ini syntax_highlighter_ini;
		jonj_get_highlight_struct(&syntax_highlighter_ini);
		highlight_file((char *)r->filename, &syntax_highlighter_ini TSRMLS_CC);
	} else {
		zend_file_handle zfd;

		zfd.type = ZEND_HANDLE_FILENAME;
		zfd.filename = (char *) r->filename;
		zfd.free_filename = 0;
		zfd.opened_path = NULL;

		if (!parent_req) {
			jonj_execute_script(&zfd TSRMLS_CC);
		} else {
			zend_execute_scripts(ZEND_INCLUDE TSRMLS_CC, NULL, 1, &zfd);
		}

		apr_table_set(r->notes, "mod_jonj_memory_usage",
			apr_psprintf(ctx->r->pool, "%" APR_SIZE_T_FMT, zend_memory_peak_usage(1 TSRMLS_CC)));
	}

} zend_end_try();

	if (!parent_req) {
		jonj_apache_request_dtor(r TSRMLS_CC);
		ctx->request_processed = 1;
		bucket = apr_bucket_eos_create(r->connection->bucket_alloc);
		APR_BRIGADE_INSERT_TAIL(brigade, bucket);

		rv = ap_pass_brigade(r->output_filters, brigade);
		if (rv != APR_SUCCESS || r->connection->aborted) {
zend_first_try {
			jonj_handle_aborted_connection();
} zend_end_try();
		}
		apr_brigade_cleanup(brigade);
		apr_pool_cleanup_run(r->pool, (void *)&SG(server_context), jonj_server_context_cleanup);
	} else {
		ctx->r = parent_req;
	}

	return OK;
}

static void jonj_apache_child_init(apr_pool_t *pchild, server_rec *s)
{
	apr_pool_cleanup_register(pchild, NULL, jonj_apache_child_shutdown, apr_pool_cleanup_null);
}

void jonj_ap2_register_hook(apr_pool_t *p)
{
	ap_hook_pre_config(jonj_pre_config, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(jonj_apache_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(jonj_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(jonj_apache_child_init, NULL, NULL, APR_HOOK_MIDDLE);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
