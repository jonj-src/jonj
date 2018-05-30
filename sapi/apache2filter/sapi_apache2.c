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

#include <fcntl.h>

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include "jonj.h"
#include "jonj_main.h"
#include "jonj_ini.h"
#include "jonj_variables.h"
#include "SAPI.h"

#include "ext/standard/jonj_smart_str.h"
#ifndef NETWARE
#include "ext/standard/jonj_standard.h"
#else
#include "ext/standard/basic_functions.h"
#endif

#include "apr_strings.h"
#include "ap_config.h"
#include "apr_buckets.h"
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

/* UnixWare and Netware define shutdown to _shutdown, which causes problems later
 * on when using a structure member named shutdown. Since this source
 * file does not use the system call shutdown, it is safe to #undef it.
 */
#undef shutdown

/* A way to specify the location of the jonj.ini dir in an apache directive */
char *apache2_jonj_ini_path_override = NULL;

static int
jonj_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	apr_bucket *b;
	apr_bucket_brigade *bb;
	apr_bucket_alloc_t *ba;
	ap_filter_t *f; /* remaining output filters */
	jonj_struct *ctx;

	ctx = SG(server_context);
	f = ctx->f;
	
	if (str_length == 0) return 0;
	
	ba = f->c->bucket_alloc;
	bb = apr_brigade_create(ctx->r->pool, ba);

	b = apr_bucket_transient_create(str, str_length, ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);

	if (ap_pass_brigade(f->next, bb) != APR_SUCCESS || ctx->r->connection->aborted) {
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

	switch(op) {
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
				sapi_free_header(sapi_header);
				return 0;
			}
			ptr = val;

			*val = '\0';
			
			do {
				val++;
			} while (*val == ' ');

			if (!strcasecmp(sapi_header->header, "content-type"))
				ctx->r->content_type = apr_pstrdup(ctx->r->pool, val);
                       else if (!strcasecmp(sapi_header->header, "content-length"))
                               ap_set_content_length(ctx->r, strtol(val, (char **)NULL, 10));
			else if (op == SAPI_HEADER_REPLACE)
				apr_table_set(ctx->r->headers_out, sapi_header->header, val);
			else
				apr_table_add(ctx->r->headers_out, sapi_header->header, val);
			
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

	ctx->r->status = SG(sapi_headers).http_response_code;

	return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
jonj_apache_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
	apr_size_t len;
	jonj_struct *ctx = SG(server_context);
	apr_bucket_brigade *brigade;
	apr_bucket *partition;

	brigade = ctx->post_data;
	len = count_bytes;

	switch (apr_brigade_partition(ctx->post_data, count_bytes, &partition)) {
	case APR_SUCCESS:
		apr_brigade_flatten(ctx->post_data, buf, &len);
		brigade = apr_brigade_split(ctx->post_data, partition);
		apr_brigade_destroy(ctx->post_data);
		ctx->post_data = brigade;
		break;
	case APR_INCOMPLETE:
		apr_brigade_flatten(ctx->post_data, buf, &len);
		apr_brigade_cleanup(ctx->post_data);
		break;
	}

	return len;
}
static struct stat*
jonj_apache_sapi_get_stat(TSRMLS_D)
{
	jonj_struct *ctx = SG(server_context);

	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
	ctx->finfo.st_dev = ctx->r->finfo.device;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
#ifdef NETWARE
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
	
	env_var = apr_table_get(ctx->r->subprocess_env, name);

	return (char *) env_var;
}

static void
jonj_apache_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	jonj_struct *ctx = SG(server_context);
	const apr_array_header_t *arr = apr_table_elts(ctx->r->subprocess_env);
	char *key, *val;
	unsigned int new_val_len;
	
	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) {
			val = "";
		}
		if (sapi_module.input_filter(PARSE_SERVER, key, &val, strlen(val), &new_val_len TSRMLS_CC)) {
			jonj_register_variable_safe(key, val, new_val_len, track_vars_array TSRMLS_CC);
		}
	APR_ARRAY_FOREACH_CLOSE()
		
	jonj_register_variable("JONJ_SELF", ctx->r->uri, track_vars_array TSRMLS_CC);
	if (sapi_module.input_filter(PARSE_SERVER, "JONJ_SELF", &ctx->r->uri, strlen(ctx->r->uri), &new_val_len TSRMLS_CC)) {
		jonj_register_variable_safe("JONJ_SELF", ctx->r->uri, new_val_len, track_vars_array TSRMLS_CC);
	}
}

static void
jonj_apache_sapi_flush(void *server_context)
{
	jonj_struct *ctx;
	apr_bucket_brigade *bb;
	apr_bucket_alloc_t *ba;
	apr_bucket *b;
	ap_filter_t *f; /* output filters */
	TSRMLS_FETCH();

	ctx = server_context;

	/* If we haven't registered a server_context yet,
	 * then don't bother flushing. */
	if (!server_context)
		return;

	sapi_send_headers(TSRMLS_C);

	ctx->r->status = SG(sapi_headers).http_response_code;
	SG(headers_sent) = 1;

	f = ctx->f;

	/* Send a flush bucket down the filter chain. The current default
	 * handler seems to act on the first flush bucket, but ignores
	 * all further flush buckets.
	 */
	
	ba = ctx->r->connection->bucket_alloc;
	bb = apr_brigade_create(ctx->r->pool, ba);
	b = apr_bucket_flush_create(ba);
	APR_BRIGADE_INSERT_TAIL(bb, b);
	if (ap_pass_brigade(f->next, bb) != APR_SUCCESS || ctx->r->connection->aborted) {
		jonj_handle_aborted_connection();
	}
}

static void jonj_apache_sapi_log_message(char *msg TSRMLS_DC)
{
	jonj_struct *ctx;

	ctx = SG(server_context);
   
	if (ctx == NULL) { /* we haven't initialized our ctx yet, oh well */
		ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_STARTUP, 0, NULL, "%s", msg);
	}
	else {
		ap_log_error(APLOG_MARK, APLOG_ERR, 0, ctx->r->server, "%s", msg);
	}
}

static int
jonj_apache_disable_caching(ap_filter_t *f)
{
	/* Identify JONJ scripts as non-cacheable, thus preventing 
	 * Apache from sending a 304 status when the browser sends
	 * If-Modified-Since header.
	 */
	f->r->no_local_copy = 1;
	
	return OK;
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
	"apache2filter",
	"Apache 2.0 Filter",

	jonj_apache2_startup,						/* startup */
	jonj_module_shutdown_wrapper,			/* shutdown */

	NULL,									/* activate */
	NULL,									/* deactivate */

	jonj_apache_sapi_ub_write,				/* unbuffered write */
	jonj_apache_sapi_flush,					/* flush */
	jonj_apache_sapi_get_stat,						/* get uid */
	jonj_apache_sapi_getenv,					/* getenv */

	jonj_error,								/* error handler */

	jonj_apache_sapi_header_handler,			/* header handler */
	jonj_apache_sapi_send_headers,			/* send headers handler */
	NULL,									/* send header handler */

	jonj_apache_sapi_read_post,				/* read POST data */
	jonj_apache_sapi_read_cookies,			/* read Cookies */

	jonj_apache_sapi_register_variables,
	jonj_apache_sapi_log_message,			/* Log message */
	jonj_apache_sapi_get_request_time,		/* Get Request Time */
	NULL,						/* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};

static int jonj_input_filter(ap_filter_t *f, apr_bucket_brigade *bb, 
		ap_input_mode_t mode, apr_read_type_e block, apr_off_t readbytes)
{
	jonj_struct *ctx;
	apr_status_t rv;
	TSRMLS_FETCH();

	if (f->r->proxyreq) {
		return ap_get_brigade(f->next, bb, mode, block, readbytes);
	}

	ctx = SG(server_context);
	if (ctx == NULL) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r,
					 "jonj failed to get server context");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	if ((rv = ap_get_brigade(f->next, bb, mode, block, readbytes)) != APR_SUCCESS) {
		return rv;
	}

	if (!ctx->post_data) {
		ctx->post_data = apr_brigade_create(f->r->pool, f->c->bucket_alloc);
	}
	if ((rv = ap_save_brigade(f, &ctx->post_data, &bb, f->r->pool)) != APR_SUCCESS) {
		return rv;
	}
	apr_brigade_cleanup(bb);
	APR_BRIGADE_INSERT_TAIL(bb, apr_bucket_eos_create(bb->bucket_alloc));

	return APR_SUCCESS;
}

static void jonj_apache_request_ctor(ap_filter_t *f, jonj_struct *ctx TSRMLS_DC)
{
	char *content_type;
	char *content_length;
	const char *auth;

	PG(during_request_startup) = 0;
	SG(sapi_headers).http_response_code = !f->r->status ? HTTP_OK : f->r->status;
	SG(request_info).content_type = apr_table_get(f->r->headers_in, "Content-Type");
#undef safe_strdup
#define safe_strdup(x) ((x)?strdup((x)):NULL)	
	SG(request_info).query_string = safe_strdup(f->r->args);
	SG(request_info).request_method = f->r->method;
	SG(request_info).proto_num = f->r->proto_num;
	SG(request_info).request_uri = safe_strdup(f->r->uri);
	SG(request_info).path_translated = safe_strdup(f->r->filename);
	f->r->no_local_copy = 1;
	content_type = sapi_get_default_content_type(TSRMLS_C);
	f->r->content_type = apr_pstrdup(f->r->pool, content_type);

	efree(content_type);

	content_length = (char *) apr_table_get(f->r->headers_in, "Content-Length");
	SG(request_info).content_length = (content_length ? atol(content_length) : 0);
	
	apr_table_unset(f->r->headers_out, "Content-Length");
	apr_table_unset(f->r->headers_out, "Last-Modified");
	apr_table_unset(f->r->headers_out, "Expires");
	apr_table_unset(f->r->headers_out, "ETag");

	auth = apr_table_get(f->r->headers_in, "Authorization");
	jonj_handle_auth_data(auth TSRMLS_CC);

	if (SG(request_info).auth_user == NULL && f->r->user) {
		SG(request_info).auth_user = estrdup(f->r->user);
	}

	ctx->r->user = apr_pstrdup(ctx->r->pool, SG(request_info).auth_user);

	jonj_request_startup(TSRMLS_C);
}

static void jonj_apache_request_dtor(ap_filter_t *f TSRMLS_DC)
{
	jonj_apr_bucket_brigade *pbb = (jonj_apr_bucket_brigade *)f->ctx;
	
	jonj_request_shutdown(NULL);

	if (SG(request_info).query_string) {
		free(SG(request_info).query_string);
	}
	if (SG(request_info).request_uri) {
		free(SG(request_info).request_uri);
	}
	if (SG(request_info).path_translated) {
		free(SG(request_info).path_translated);
	}
	
	apr_brigade_destroy(pbb->bb);
}

static int jonj_output_filter(ap_filter_t *f, apr_bucket_brigade *bb)
{
	jonj_struct *ctx;
	void *conf = ap_get_module_config(f->r->per_dir_config, &jonj5_module);
	char *p = get_jonj_config(conf, "engine", sizeof("engine"));
	zend_file_handle zfd;
	jonj_apr_bucket_brigade *pbb;
	apr_bucket *b;
	TSRMLS_FETCH();
	
	if (f->r->proxyreq) {
		zend_try {
			zend_ini_deactivate(TSRMLS_C);
		} zend_end_try();
		return ap_pass_brigade(f->next, bb);
	}
	
	/* handle situations where user turns the engine off */
	if (*p == '0') {
		zend_try {
			zend_ini_deactivate(TSRMLS_C);
		} zend_end_try();
		return ap_pass_brigade(f->next, bb);
	}	
	
	if(f->ctx) {
		pbb = (jonj_apr_bucket_brigade *)f->ctx;
	} else {
		pbb = f->ctx = apr_palloc(f->r->pool, sizeof(*pbb));
		pbb->bb = apr_brigade_create(f->r->pool, f->c->bucket_alloc);
	}

	if(ap_save_brigade(NULL, &pbb->bb, &bb, f->r->pool) != APR_SUCCESS) {
		/* Bad */
	}
	
	apr_brigade_cleanup(bb);
	
	/* Check to see if the last bucket in this brigade, it not
	 * we have to wait until then. */
	if(!APR_BUCKET_IS_EOS(APR_BRIGADE_LAST(pbb->bb))) {
		return 0;
	}	
	
	/* Setup the CGI variables if this is the main request.. */
	if (f->r->main == NULL || 
		/* .. or if the sub-request envinronment differs from the main-request. */
		f->r->subprocess_env != f->r->main->subprocess_env
	) {
		/* setup standard CGI variables */
		ap_add_common_vars(f->r);
		ap_add_cgi_vars(f->r);
	}
	
	ctx = SG(server_context);
	if (ctx == NULL) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r,
					 "jonj failed to get server context");
		zend_try {
			zend_ini_deactivate(TSRMLS_C);
		} zend_end_try();
        return HTTP_INTERNAL_SERVER_ERROR;
	}
	
	ctx->f = f->next; /* save whatever filters are after us in the chain. */

	if (ctx->request_processed) {
		zend_try {
			zend_ini_deactivate(TSRMLS_C);
		} zend_end_try();
		return ap_pass_brigade(f->next, bb);
	}

	apply_config(conf);
	jonj_apache_request_ctor(f, ctx TSRMLS_CC);
	
	/* It'd be nice if we could highlight based of a zend_file_handle here....
	 * ...but we can't. */
	
	zfd.type = ZEND_HANDLE_STREAM;
	
	zfd.handle.stream.handle = pbb;
	zfd.handle.stream.reader = jonj_apache_read_stream;
	zfd.handle.stream.closer = NULL;
	zfd.handle.stream.fsizer = jonj_apache_fsizer_stream;
	zfd.handle.stream.isatty = 0;
	
	zfd.filename = f->r->filename;
	zfd.opened_path = NULL;
	zfd.free_filename = 0;
	
	jonj_execute_script(&zfd TSRMLS_CC);

	apr_table_set(ctx->r->notes, "mod_jonj_memory_usage",
		apr_psprintf(ctx->r->pool, "%lu", (unsigned long) zend_memory_peak_usage(1 TSRMLS_CC)));
		
	jonj_apache_request_dtor(f TSRMLS_CC);
		
	if (!f->r->main) {
		ctx->request_processed = 1;
	}
	
	b = apr_bucket_eos_create(f->c->bucket_alloc);
	APR_BRIGADE_INSERT_TAIL(pbb->bb,  b);
	
	/* Pass whatever is left on the brigade. */
	return ap_pass_brigade(f->next, pbb->bb);
}

static apr_status_t
jonj_apache_server_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
	sapi_shutdown();
#ifdef ZTS
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
jonj_apache_server_startup(apr_pool_t *pconf, apr_pool_t *plog,
                          apr_pool_t *ptemp, server_rec *s)
{
	void *data = NULL;
	const char *userdata_key = "apache2filter_post_config";

	/* Apache will load, unload and then reload a DSO module. This
	 * prevents us from starting JONJ until the second load. */
	apr_pool_userdata_get(&data, userdata_key, s->process->pool);
	if (data == NULL) {
		/* We must use set() here and *not* setn(), otherwise the
		 * static string pointed to by userdata_key will be mapped
		 * to a different location when the DSO is reloaded and the
		 * pointers won't match, causing get() to return NULL when
		 * we expected it to return non-NULL. */
		apr_pool_userdata_set((const void *)1, userdata_key,
							  apr_pool_cleanup_null, s->process->pool);
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

static void jonj_add_filter(request_rec *r, ap_filter_t *f)
{
	int output = (f == r->output_filters);

	/* for those who still have Set*Filter JONJ configured */
	while (f) {
		if (strcmp(f->frec->name, "JONJ") == 0) {
			ap_log_error(APLOG_MARK, APLOG_WARNING,
				     0, r->server,
				     "\"Set%sFilter JONJ\" already configured for %s",
				     output ? "Output" : "Input", r->uri);
			return;
		}
		f = f->next;
	}

	if (output) {
		ap_add_output_filter("JONJ", NULL, r, r->connection);
	} else {
		ap_add_input_filter("JONJ", NULL, r, r->connection);
	}
}

static void jonj_insert_filter(request_rec *r)
{
	int content_type_len = strlen("application/x-httpd-jonj");

	if (r->content_type && !strncmp(r->content_type, "application/x-httpd-jonj", content_type_len-1)) {
		if (r->content_type[content_type_len] == '\0' || !strncmp(r->content_type+content_type_len, "-source", sizeof("-source"))) { 
			jonj_add_filter(r, r->output_filters);
			jonj_add_filter(r, r->input_filters);
		}	
	}
}

static apr_status_t jonj_server_context_cleanup(void *data_)
{
	void **data = data_;
	*data = NULL;
	return APR_SUCCESS;
}

static int jonj_post_read_request(request_rec *r)
{
	jonj_struct *ctx;
	TSRMLS_FETCH();

	/* Initialize filter context */
	SG(server_context) = ctx = apr_pcalloc(r->pool, sizeof(*ctx));

	/* register a cleanup so we clear out the SG(server_context)
	 * after each request. Note: We pass in the pointer to the
	 * server_context in case this is handled by a different thread. */
	apr_pool_cleanup_register(r->pool, (void *)&SG(server_context),
							  jonj_server_context_cleanup,
							  apr_pool_cleanup_null);

	/* Save the entire request, so we can get the input or output
	 * filters if we need them. */
	ctx->r = r;

	return OK;
}

static void jonj_register_hook(apr_pool_t *p)
{
	ap_hook_pre_config(jonj_pre_config, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(jonj_apache_server_startup, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_insert_filter(jonj_insert_filter, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_read_request(jonj_post_read_request, NULL, NULL, APR_HOOK_MIDDLE);
	ap_register_output_filter("JONJ", jonj_output_filter, jonj_apache_disable_caching, AP_FTYPE_RESOURCE);
	ap_register_input_filter("JONJ", jonj_input_filter, jonj_apache_disable_caching, AP_FTYPE_RESOURCE);
}

static size_t jonj_apache_read_stream(void *handle, char *buf, size_t wantlen TSRMLS_DC)
{
	jonj_apr_bucket_brigade *pbb = (jonj_apr_bucket_brigade *)handle;
	apr_bucket_brigade *rbb;
	apr_size_t readlen;
	apr_bucket *b = NULL;
	
	rbb = pbb->bb;
	
	if((apr_brigade_partition(pbb->bb, wantlen, &b) == APR_SUCCESS) && b){
		pbb->bb = apr_brigade_split(rbb, b);
	} 	

	readlen = wantlen;
	apr_brigade_flatten(rbb, buf, &readlen);
	apr_brigade_cleanup(rbb);
	
	return readlen;
}

static size_t jonj_apache_fsizer_stream(void *handle TSRMLS_DC)
{
	jonj_apr_bucket_brigade *pbb = (jonj_apr_bucket_brigade *)handle;
	apr_off_t actual = 0;

	if (apr_brigade_length(pbb->bb, 1, &actual) == APR_SUCCESS) {
		return actual;
	}

	return 0;
}

AP_MODULE_DECLARE_DATA module jonj5_module = {
	STANDARD20_MODULE_STUFF,
	create_jonj_config,		/* create per-directory config structure */
	merge_jonj_config,		/* merge per-directory config structures */
	NULL,					/* create per-server config structure */
	NULL,					/* merge per-server config structures */
	jonj_dir_cmds,			/* command apr_table_t */
	jonj_register_hook		/* register hooks */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
