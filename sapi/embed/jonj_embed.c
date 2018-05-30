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
   | Author: Edin Kadribasic <edink@jonj.tk>                              |
   +----------------------------------------------------------------------+
*/
/* $Id$ */

#include "jonj_embed.h"
#include "ext/standard/jonj_standard.h"

#ifdef JONJ_WIN32
#include <io.h>
#include <fcntl.h>
#endif

const char HARDCODED_INI[] =
	"html_errors=0\n"
	"register_argc_argv=1\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n\0";

static char* jonj_embed_read_cookies(TSRMLS_D)
{
	return NULL;
}

static int jonj_embed_deactivate(TSRMLS_D)
{
	fflush(stdout);
	return SUCCESS;
}

static inline size_t jonj_embed_single_write(const char *str, uint str_length)
{
#ifdef JONJ_WRITE_STDOUT
	long ret;

	ret = write(STDOUT_FILENO, str, str_length);
	if (ret <= 0) return 0;
	return ret;
#else
	size_t ret;

	ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
	return ret;
#endif
}


static int jonj_embed_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	const char *ptr = str;
	uint remaining = str_length;
	size_t ret;

	while (remaining > 0) {
		ret = jonj_embed_single_write(ptr, remaining);
		if (!ret) {
			jonj_handle_aborted_connection();
		}
		ptr += ret;
		remaining -= ret;
	}

	return str_length;
}

static void jonj_embed_flush(void *server_context)
{
	if (fflush(stdout)==EOF) {
		jonj_handle_aborted_connection();
	}
}

static void jonj_embed_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
}

static void jonj_embed_log_message(char *message TSRMLS_DC)
{
	fprintf (stderr, "%s\n", message);
}

static void jonj_embed_register_variables(zval *track_vars_array TSRMLS_DC)
{
	jonj_import_environment_variables(track_vars_array TSRMLS_CC);
}

static int jonj_embed_startup(sapi_module_struct *sapi_module)
{
	if (jonj_module_startup(sapi_module, NULL, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

extern EMBED_SAPI_API sapi_module_struct jonj_embed_module = {
	"embed",                       /* name */
	"JONJ Embedded Library",        /* pretty name */
	
	jonj_embed_startup,              /* startup */
	jonj_module_shutdown_wrapper,   /* shutdown */
  
	NULL,                          /* activate */
	jonj_embed_deactivate,           /* deactivate */
  
	jonj_embed_ub_write,             /* unbuffered write */
	jonj_embed_flush,                /* flush */
	NULL,                          /* get uid */
	NULL,                          /* getenv */
  
	jonj_error,                     /* error handler */
  
	NULL,                          /* header handler */
	NULL,                          /* send headers handler */
	jonj_embed_send_header,          /* send header handler */
	
	NULL,                          /* read POST data */
	jonj_embed_read_cookies,         /* read Cookies */
  
	jonj_embed_register_variables,   /* register server variables */
	jonj_embed_log_message,          /* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */
  
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

/* {{{ arginfo ext/standard/dl.c */
ZEND_BEGIN_ARG_INFO(arginfo_dl, 0)
	ZEND_ARG_INFO(0, extension_filename)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry additional_functions[] = {
	ZEND_FE(dl, arginfo_dl)
	{NULL, NULL, NULL}
};

EMBED_SAPI_API int jonj_embed_init(int argc, char **argv PTSRMLS_DC)
{
	zend_llist global_vars;
#ifdef ZTS
	void ***tsrm_ls = NULL;
#endif

#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
								 that sockets created via fsockopen()
								 don't kill JONJ if the remote site
								 closes it.  in apache|apxs mode apache
								 does that for us!  thies@thieso.net
								 20000419 */
#endif
#endif

#ifdef ZTS
  tsrm_startup(1, 1, 0, NULL);
  tsrm_ls = ts_resource(0);
  *ptsrm_ls = tsrm_ls;
#endif

  sapi_startup(&jonj_embed_module);

#ifdef JONJ_WIN32
  _fmode = _O_BINARY;			/*sets default for file streams to binary */
  setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
  setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
  setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif

  jonj_embed_module.ini_entries = malloc(sizeof(HARDCODED_INI));
  memcpy(jonj_embed_module.ini_entries, HARDCODED_INI, sizeof(HARDCODED_INI));

  jonj_embed_module.additional_functions = additional_functions;

  if (argv) {
	jonj_embed_module.executable_location = argv[0];
  }

  if (jonj_embed_module.startup(&jonj_embed_module)==FAILURE) {
	  return FAILURE;
  }
 
  zend_llist_init(&global_vars, sizeof(char *), NULL, 0);  

  /* Set some Embedded JONJ defaults */
  SG(options) |= SAPI_OPTION_NO_CHDIR;
  SG(request_info).argc=argc;
  SG(request_info).argv=argv;

  if (jonj_request_startup(TSRMLS_C)==FAILURE) {
	  jonj_module_shutdown(TSRMLS_C);
	  return FAILURE;
  }
  
  SG(headers_sent) = 1;
  SG(request_info).no_headers = 1;
  jonj_register_variable("JONJ_SELF", "-", NULL TSRMLS_CC);

  return SUCCESS;
}

EMBED_SAPI_API void jonj_embed_shutdown(TSRMLS_D)
{
	jonj_request_shutdown((void *) 0);
	jonj_module_shutdown(TSRMLS_C);
	sapi_shutdown();
#ifdef ZTS
    tsrm_shutdown();
#endif
	if (jonj_embed_module.ini_entries) {
		free(jonj_embed_module.ini_entries);
		jonj_embed_module.ini_entries = NULL;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
