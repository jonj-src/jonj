
	/* $Id: fpm_jonj.h,v 1.10.2.1 2008/11/15 00:57:24 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_JONJ_H
#define FPM_JONJ_H 1

#include <TSRM.h>

#include "jonj.h"
#include "build-defs.h" /* for JONJ_ defines */
#include "fpm/fpm_conf.h"

#define FPM_JONJ_INI_TO_EXPAND \
	{ \
		"error_log", \
		"extension_dir", \
		"mime_magic.magicfile", \
		"sendmail_path", \
		"session.cookie_path", \
		"session_pgsql.sem_file_name", \
		"soap.wsdl_cache_dir", \
		"uploadprogress.file.filename_template", \
		"xdebug.output_dir", \
		"xdebug.profiler_output_dir", \
		"xdebug.trace_output_dir", \
		"xmms.path", \
		"axis2.client_home", \
		"blenc.key_file", \
		"coin_acceptor.device", \
		NULL \
	}

struct fpm_worker_pool_s;

int fpm_jonj_init_child(struct fpm_worker_pool_s *wp);
char *fpm_jonj_script_filename(TSRMLS_D);
char *fpm_jonj_request_uri(TSRMLS_D);
char *fpm_jonj_request_method(TSRMLS_D);
char *fpm_jonj_query_string(TSRMLS_D);
char *fpm_jonj_auth_user(TSRMLS_D);
size_t fpm_jonj_content_length(TSRMLS_D);
void fpm_jonj_soft_quit();
int fpm_jonj_init_main();
int fpm_jonj_apply_defines_ex(struct key_value_s *kv, int mode);
int fpm_jonj_limit_extensions(char *path);
char* fpm_jonj_get_string_from_table(char *table, char *key TSRMLS_DC);

#endif

