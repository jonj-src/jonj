/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The JONJ Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at the following url:        |
   | http://www.jonj.tk/license/3_01.txt.                                 |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: George Wang <gwang@litespeedtech.com>                        |
   +----------------------------------------------------------------------+
*/

#include "jonj.h"
#include "SAPI.h"
#include "jonj_main.h"
#include "jonj_ini.h"
#include "jonj_variables.h"
#include "zend_highlight.h"
#include "zend.h"

#include "lsapilib.h"

#include <stdio.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef JONJ_WIN32

#include <io.h>
#include <fcntl.h>
#include "win32/jonj_registry.h"

#else

#include <sys/wait.h>

#endif

#include <sys/stat.h>

#if HAVE_SYS_TYPES_H

#include <sys/types.h>

#endif

#if HAVE_SIGNAL_H

#include <signal.h>

#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define SAPI_LSAPI_MAX_HEADER_LENGTH 2048

/* Key for each cache entry is dirname(PATH_TRANSLATED).
 *
 * NOTE: Each cache entry config_hash contains the combination from all user ini files found in
 *       the path starting from doc_root throught to dirname(PATH_TRANSLATED).  There is no point
 *       storing per-file entries as it would not be possible to detect added / deleted entries
 *       between separate files.
 */
typedef struct _user_config_cache_entry {
    time_t expires;
    HashTable user_config;
} user_config_cache_entry;
static HashTable user_config_cache;

static int  lsapi_mode       = 0;
static char *jonj_self        = "";
static char *script_filename = "";
static int  source_highlight = 0;
static int  ignore_jonj_ini   = 0;
static char * argv0 = NULL;
static int  engine = 1;
static int  parse_user_ini   = 0;
#ifdef ZTS
zend_compiler_globals    *compiler_globals;
zend_executor_globals    *executor_globals;
jonj_core_globals         *core_globals;
sapi_globals_struct      *sapi_globals;
void ***tsrm_ls;
#endif

zend_module_entry litespeed_module_entry;

static void init_sapi_from_env(sapi_module_struct *sapi_module)
{
    char *p;
    p = getenv("LSJONJRC");
    if (p)
        sapi_module->jonj_ini_path_override = p;
}

/* {{{ jonj_lsapi_startup
 */
static int jonj_lsapi_startup(sapi_module_struct *sapi_module)
{
    if (jonj_module_startup(sapi_module, NULL, 0)==FAILURE) {
        return FAILURE;
    }
    argv0 = sapi_module->executable_location;
    return SUCCESS;
}
/* }}} */

/* {{{ sapi_lsapi_ini_defaults */

/* overwriteable ini defaults must be set in sapi_cli_ini_defaults() */
#define INI_DEFAULT(name,value)\
    ZVAL_STRING(tmp, value, 0);\
    zend_hash_update(configuration_hash, name, sizeof(name), tmp, sizeof(zval), (void**)&entry);\
    Z_STRVAL_P(entry) = zend_strndup(Z_STRVAL_P(entry), Z_STRLEN_P(entry))

static void sapi_lsapi_ini_defaults(HashTable *configuration_hash)
{
    zval *tmp, *entry;

#if JONJ_MAJOR_VERSION > 4
/*
    MAKE_STD_ZVAL(tmp);

    INI_DEFAULT("register_long_arrays", "0");

    FREE_ZVAL(tmp);
*/
#endif

}
/* }}} */


/* {{{ sapi_lsapi_ub_write
 */
#if JONJ_MAJOR_VERSION >= 7
static size_t sapi_lsapi_ub_write(const char *str, size_t str_length TSRMLS_DC)
#else
static int sapi_lsapi_ub_write(const char *str, uint str_length TSRMLS_DC)
#endif
{
    int ret;
    int remain;
    if ( lsapi_mode ) {
        ret  = LSAPI_Write( str, str_length );
        if ( ret < str_length ) {
            jonj_handle_aborted_connection();
            return str_length - ret;
        }
    } else {
        remain = str_length;
        while( remain > 0 ) {
            ret = write( 1, str, remain );
            if ( ret <= 0 ) {
                jonj_handle_aborted_connection();
                return str_length - remain;
            }
            str += ret;
            remain -= ret;
        }
    }
    return str_length;
}
/* }}} */


/* {{{ sapi_lsapi_flush
 */
static void sapi_lsapi_flush(void * server_context)
{
    if ( lsapi_mode ) {
        if ( LSAPI_Flush() == -1) {
            jonj_handle_aborted_connection();
        }
    }
}
/* }}} */


/* {{{ sapi_lsapi_deactivate
 */
static int sapi_lsapi_deactivate(TSRMLS_D)
{
    if ( SG(request_info).path_translated )
    {
        efree( SG(request_info).path_translated );
    }

    return SUCCESS;
}
/* }}} */




/* {{{ sapi_lsapi_getenv
 */
static char *sapi_lsapi_getenv( char * name, size_t name_len TSRMLS_DC )
{
    if ( lsapi_mode ) {
        return LSAPI_GetEnv( name );
    } else {
        return getenv( name );
    }
}
/* }}} */


#if JONJ_MAJOR_VERSION > 4

static int add_variable( const char * pKey, int keyLen, const char * pValue, int valLen,
                         void * arg TSRMLS_DC)
{
#if JONJ_MAJOR_VERSION >= 7
    int filter_arg = (Z_ARR_P((zval *)arg) == Z_ARR(PG(http_globals)[TRACK_VARS_ENV]))
        ? PARSE_ENV : PARSE_SERVER;
    size_t new_val_len;
#else
    int filter_arg = (arg == PG(http_globals)[TRACK_VARS_ENV])?PARSE_ENV:PARSE_SERVER;
    unsigned int new_val_len;
#endif

    char * new_val = (char *) pValue; 

    if (sapi_module.input_filter(filter_arg, (char *)pKey, &new_val, valLen, &new_val_len TSRMLS_CC)) {
        jonj_register_variable_safe((char *)pKey, new_val, new_val_len, (zval *)arg TSRMLS_CC);
    }
    return 1;
}

#else

static int add_variable( const char * pKey, int keyLen, const char * pValue, int valLen,
                         void * arg )
{
    zval * gpc_element, **gpc_element_p;
    HashTable * symtable1 = Z_ARRVAL_P((zval * )arg);
    register char * pKey1 = (char *)pKey;

    MAKE_STD_ZVAL(gpc_element);
    Z_STRLEN_P( gpc_element ) = valLen;
    Z_STRVAL_P( gpc_element ) = estrndup(pValue, valLen);
    Z_TYPE_P( gpc_element ) = IS_STRING;
    zend_hash_update( symtable1, pKey1, keyLen + 1, &gpc_element, sizeof( zval *), (void **) &gpc_element_p );
    return 1;
}
#endif


#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
struct param_zts
{
    void *arg;
    TSRMLS_D ;
};

static int add_variable_zts(const char * pKey, int keyLen, const char * pValue, int valLen,
                         void * arg )
{
    struct param_zts * params = (struct param_zts *)arg;
    return add_variable(pKey, keyLen, pValue, valLen, params->arg, params->TSRMLS_C);
}
#endif

static void litespeed_jonj_import_environment_variables(zval *array_ptr TSRMLS_DC)
{
	char buf[128];
	char **env, *p, *t = buf;
	size_t alloc_size = sizeof(buf);
	unsigned long nlen; /* ptrdiff_t is not portable */

#if JONJ_MAJOR_VERSION >= 7
    if (Z_TYPE(PG(http_globals)[TRACK_VARS_ENV]) == IS_ARRAY &&
        Z_ARR_P(array_ptr) != Z_ARR(PG(http_globals)[TRACK_VARS_ENV]) &&
        zend_hash_num_elements(Z_ARRVAL(PG(http_globals)[TRACK_VARS_ENV])) > 0
	) {
        zval_dtor(array_ptr);
        ZVAL_DUP(array_ptr, &PG(http_globals)[TRACK_VARS_ENV]);
		return;
    } else if (Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY &&
        Z_ARR_P(array_ptr) != Z_ARR(PG(http_globals)[TRACK_VARS_SERVER]) &&
        zend_hash_num_elements(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER])) > 0
	) {
        zval_dtor(array_ptr);
        ZVAL_DUP(array_ptr, &PG(http_globals)[TRACK_VARS_SERVER]);
		return;
	}
#else
    if (PG(http_globals)[TRACK_VARS_ENV] &&
        array_ptr != PG(http_globals)[TRACK_VARS_ENV] &&
        Z_TYPE_P(PG(http_globals)[TRACK_VARS_ENV]) == IS_ARRAY &&
        zend_hash_num_elements(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_ENV])) > 0
    ) {
        zval_dtor(array_ptr);
        *array_ptr = *PG(http_globals)[TRACK_VARS_ENV];
        INIT_PZVAL(array_ptr);
        zval_copy_ctor(array_ptr);
        return;
    } else if (PG(http_globals)[TRACK_VARS_SERVER] &&
        array_ptr != PG(http_globals)[TRACK_VARS_SERVER] &&
        Z_TYPE_P(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY &&
        zend_hash_num_elements(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER])) > 0
    ) {
        zval_dtor(array_ptr);
        *array_ptr = *PG(http_globals)[TRACK_VARS_SERVER];
        INIT_PZVAL(array_ptr);
        zval_copy_ctor(array_ptr);
        return;
    }
#endif

	for (env = environ; env != NULL && *env != NULL; env++) {
		p = strchr(*env, '=');
		if (!p) {				/* malformed entry? */
			continue;
		}
		nlen = p - *env;
		if (nlen >= alloc_size) {
			alloc_size = nlen + 64;
			t = (t == buf ? emalloc(alloc_size): erealloc(t, alloc_size));
		}
		memcpy(t, *env, nlen);
		t[nlen] = '\0';
		add_variable(t, nlen, p + 1, strlen( p + 1 ), array_ptr TSRMLS_CC);
	}
	if (t != buf && t != NULL) {
		efree(t);
	}
}


#if ((JONJ_MAJOR_VERSION == 5 && JONJ_MINOR_VERSION < 4) || JONJ_MAJOR_VERSION < 5)
static int add_variable_magic_quote( const char * pKey, int keyLen, const char * pValue, int valLen, 
                         void * arg )
{
    zval * gpc_element, **gpc_element_p;
    HashTable * symtable1 = Z_ARRVAL_P((zval * )arg);
    register char * pKey1 = (char *)pKey;

    MAKE_STD_ZVAL(gpc_element);
    Z_STRLEN_P( gpc_element ) = valLen;
    Z_STRVAL_P( gpc_element ) = jonj_addslashes((char *)pValue, valLen, &Z_STRLEN_P( gpc_element ), 0 );
    Z_TYPE_P( gpc_element ) = IS_STRING;
#if JONJ_MAJOR_VERSION > 4
    zend_symtable_update( symtable1, pKey1, keyLen + 1, &gpc_element, sizeof( zval *), (void **) &gpc_element_p );
#else
    zend_hash_update( symtable1, pKey1, keyLen + 1, &gpc_element, sizeof( zval *), (void **) &gpc_element_p );
#endif
    return 1;
}

#endif

/* {{{ sapi_lsapi_register_variables
 */
static void sapi_lsapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
    char * jonj_self = "";
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
    struct param_zts params;
    params.arg = track_vars_array;
    params.TSRMLS_C = TSRMLS_C;
#endif
    if ( lsapi_mode ) {
        if ( (SG(request_info).request_uri ) )
            jonj_self = (SG(request_info).request_uri );

        litespeed_jonj_import_environment_variables(track_vars_array TSRMLS_CC);

#if ((JONJ_MAJOR_VERSION == 5 && JONJ_MINOR_VERSION < 4) || JONJ_MAJOR_VERSION < 5)
        if (!PG(magic_quotes_gpc)) {
#endif
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
            LSAPI_ForeachHeader( add_variable_zts, &params );
            LSAPI_ForeachEnv( add_variable_zts, &params );
#else
            LSAPI_ForeachHeader( add_variable, track_vars_array );
            LSAPI_ForeachEnv( add_variable, track_vars_array );
#endif            
            add_variable("JONJ_SELF", 8, jonj_self, strlen( jonj_self ), 
                         track_vars_array TSRMLS_CC);
#if ((JONJ_MAJOR_VERSION == 5 && JONJ_MINOR_VERSION < 4) || JONJ_MAJOR_VERSION < 5)
        } else {
            LSAPI_ForeachHeader( add_variable_magic_quote, track_vars_array );
            LSAPI_ForeachEnv( add_variable_magic_quote, track_vars_array );
            add_variable_magic_quote("JONJ_SELF", 8, jonj_self, strlen( jonj_self ), track_vars_array );
        }
#endif
    } else {
        jonj_import_environment_variables(track_vars_array TSRMLS_CC);

        jonj_register_variable("JONJ_SELF", jonj_self, track_vars_array TSRMLS_CC);
        jonj_register_variable("SCRIPT_NAME", jonj_self, track_vars_array TSRMLS_CC);
        jonj_register_variable("SCRIPT_FILENAME", script_filename, track_vars_array TSRMLS_CC);
        jonj_register_variable("PATH_TRANSLATED", script_filename, track_vars_array TSRMLS_CC);
        jonj_register_variable("DOCUMENT_ROOT", "", track_vars_array TSRMLS_CC);

    }
}
/* }}} */


/* {{{ sapi_lsapi_read_post
 */
#if JONJ_MAJOR_VERSION >= 7
static size_t sapi_lsapi_read_post(char *buffer, size_t count_bytes TSRMLS_DC)
#else
static int sapi_lsapi_read_post(char *buffer, uint count_bytes TSRMLS_DC)
#endif
{
    if ( lsapi_mode ) {
        return LSAPI_ReadReqBody( buffer, (unsigned long long)count_bytes );
    } else {
        return 0;
    }
}
/* }}} */




/* {{{ sapi_lsapi_read_cookies
 */
static char *sapi_lsapi_read_cookies(TSRMLS_D)
{
    if ( lsapi_mode ) {
        return LSAPI_GetHeader( H_COOKIE );
    } else {
        return NULL;
    }
}
/* }}} */


/* {{{ sapi_lsapi_send_headers
 */
static int sapi_lsapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
    sapi_header_struct  *h;
    zend_llist_position pos;
    if ( lsapi_mode ) {
        LSAPI_SetRespStatus( SG(sapi_headers).http_response_code );

        h = zend_llist_get_first_ex(&sapi_headers->headers, &pos);
        while (h) {
            if ( h->header_len > 0 ) {
                LSAPI_AppendRespHeader(h->header, h->header_len);
            }
            h = zend_llist_get_next_ex(&sapi_headers->headers, &pos);
        }
        if (SG(sapi_headers).send_default_content_type) {
            char    *hd;
            int     len;
            char    headerBuf[SAPI_LSAPI_MAX_HEADER_LENGTH];

            hd = sapi_get_default_content_type(TSRMLS_C);
            len = snprintf( headerBuf, SAPI_LSAPI_MAX_HEADER_LENGTH - 1,
                            "Content-type: %s", hd );
            efree(hd);

            LSAPI_AppendRespHeader( headerBuf, len );
        }
    }
    LSAPI_FinalizeRespHeaders();
    return SAPI_HEADER_SENT_SUCCESSFULLY;


}
/* }}} */


/* {{{ sapi_lsapi_send_headers
 */
static void sapi_lsapi_log_message(char *message
#if JONJ_MAJOR_VERSION > 7 || (JONJ_MAJOR_VERSION == 7 && JONJ_MINOR_VERSION >= 1)
                                   , int syslog_type_int   /* unused */
#endif
                                   TSRMLS_DC)
{
    char buf[8192];
    int len = strlen( message );
    if ( *(message + len - 1 ) != '\n' )
    {
        snprintf( buf, 8191, "%s\n", message );
        message = buf;
        ++len;
    }
    LSAPI_Write_Stderr( message, len);
}
/* }}} */

/* Set to 1 to turn on log messages useful during development:
 */
#if 0
static void log_message (const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[0x100];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    sapi_lsapi_log_message(buf
#if JONJ_MAJOR_VERSION > 7 || (JONJ_MAJOR_VERSION == 7 && JONJ_MINOR_VERSION >= 1)
                               , 0
#endif
                                  );
}
#define DEBUG_MESSAGE(fmt, ...) log_message("LS:%d " fmt "\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_MESSAGE(fmt, ...)
#endif

/* {{{ sapi_module_struct cgi_sapi_module
 */
static sapi_module_struct lsapi_sapi_module =
{
    "litespeed",
    "LiteSpeed V6.10",

    jonj_lsapi_startup,              /* startup */
    jonj_module_shutdown_wrapper,    /* shutdown */

    NULL,                           /* activate */
    sapi_lsapi_deactivate,          /* deactivate */

    sapi_lsapi_ub_write,            /* unbuffered write */
    sapi_lsapi_flush,               /* flush */
    NULL,                           /* get uid */
    sapi_lsapi_getenv,              /* getenv */

    jonj_error,                      /* error handler */

    NULL,                           /* header handler */
    sapi_lsapi_send_headers,        /* send headers handler */
    NULL,                           /* send header handler */

    sapi_lsapi_read_post,           /* read POST data */
    sapi_lsapi_read_cookies,        /* read Cookies */

    sapi_lsapi_register_variables,  /* register server variables */
    sapi_lsapi_log_message,         /* Log message */

#if JONJ_MAJOR_VERSION > 7 || (JONJ_MAJOR_VERSION == 7 && JONJ_MINOR_VERSION >= 1)
    NULL,                           /* Get request time */
    NULL,                           /* Child terminate */
#else
    NULL,                           /* jonj.ini path override */
    NULL,                           /* block interruptions */
    NULL,                           /* unblock interruptions */
    NULL,                           /* default post reader */
    NULL,                           /* treat data */
    NULL,                           /* executable location */

    0,                              /* jonj.ini ignore */
#endif

    STANDARD_SAPI_MODULE_PROPERTIES

};
/* }}} */

static void init_request_info( TSRMLS_D )
{
    char * pContentType = LSAPI_GetHeader( H_CONTENT_TYPE );
    char * pAuth;
    
    SG(request_info).content_type = pContentType ? pContentType : "";
    SG(request_info).request_method = LSAPI_GetRequestMethod();
    SG(request_info).query_string = LSAPI_GetQueryString();
    SG(request_info).request_uri = LSAPI_GetScriptName();
    SG(request_info).content_length = LSAPI_GetReqBodyLen();
    SG(request_info).path_translated = estrdup( LSAPI_GetScriptFileName());

    /* It is not reset by zend engine, set it to 200. */
    SG(sapi_headers).http_response_code = 200;
    
    pAuth = LSAPI_GetHeader( H_AUTHORIZATION );
    jonj_handle_auth_data(pAuth TSRMLS_CC);
}

static char s_cur_chdir[4096] = "";

static int lsapi_chdir_primary_script( zend_file_handle * file_handle )
{
#if JONJ_MAJOR_VERSION > 4
    char * p;
    char ch;

    SG(options) |= SAPI_OPTION_NO_CHDIR;
    getcwd( s_cur_chdir, sizeof( s_cur_chdir ) );

    p = strrchr( file_handle->filename, '/' );
    if ( *p )
    {
        *p = 0;
        if ( strcmp( file_handle->filename, s_cur_chdir ) != 0 ) {
            chdir( file_handle->filename );
        }
        *p++ = '/';
        ch = *p;
        *p = 0;
        if ( !CWDG(cwd).cwd ||
             ( strcmp( file_handle->filename, CWDG(cwd).cwd ) != 0 ) ) {
            CWDG(cwd).cwd_length = p - file_handle->filename;
            CWDG(cwd).cwd = (char *) realloc(CWDG(cwd).cwd, CWDG(cwd).cwd_length+1);            
            memmove( CWDG(cwd).cwd, file_handle->filename, CWDG(cwd).cwd_length+1 );
        }
        *p = ch;
    }
    /* virtual_file_ex(&CWDG(cwd), file_handle->filename, NULL, CWD_REALPATH); */
#else
    VCWD_CHDIR_FILE( file_handle->filename );
#endif
    return 0;
}

static int lsapi_fopen_primary_script( zend_file_handle * file_handle )
{
    FILE * fp;
    char * p;
    fp = fopen( SG(request_info).path_translated, "rb" );
    if ( !fp )
    {
        return -1;
    }
    file_handle->type = ZEND_HANDLE_FP;
    file_handle->handle.fp = fp;
    file_handle->filename = SG(request_info).path_translated;
    file_handle->free_filename = 0;
    file_handle->opened_path = NULL;

    lsapi_chdir_primary_script( file_handle );

    return 0;
}

static int lsapi_execute_script( zend_file_handle * file_handle TSRMLS_DC)
{
    char *p;
    int len;
    file_handle->type = ZEND_HANDLE_FILENAME;
    file_handle->handle.fd = 0;
    file_handle->filename = SG(request_info).path_translated;
    file_handle->free_filename = 0;
    file_handle->opened_path = NULL;

    p = argv0;
    *p++ = ':';
    len = strlen( SG(request_info).path_translated );
    if ( len > 45 )
        len = len - 45;
    else
        len = 0;
    memccpy( p, SG(request_info).path_translated + len, 0, 46 );

    jonj_execute_script(file_handle TSRMLS_CC);
    return 0;

}

static int lsapi_activate_user_ini(TSRMLS_D);

static int lsapi_module_main(int show_source TSRMLS_DC)
{
    zend_file_handle file_handle = {0};

    if (jonj_request_startup(TSRMLS_C) == FAILURE ) {
        return -1;
    }
    
#if JONJ_MAJOR_VERSION > 5 || (JONJ_MAJOR_VERSION == 5 && JONJ_MINOR_VERSION >= 3)
    if (parse_user_ini && lsapi_activate_user_ini(TSRMLS_C) == FAILURE) {
        return -1;
    }
#endif    
    if (show_source) {
        zend_syntax_highlighter_ini syntax_highlighter_ini;

        jonj_get_highlight_struct(&syntax_highlighter_ini);
        highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC);
    } else {
        lsapi_execute_script( &file_handle TSRMLS_CC);
    }
    zend_try {
        jonj_request_shutdown(NULL);
        memset( argv0, 0, 46 );
    } zend_end_try();
    return 0;
}


static int alter_ini( const char * pKey, int keyLen, const char * pValue, int valLen,
                void * arg )
{
#if JONJ_MAJOR_VERSION >= 7
	zend_string * psKey; 
#endif
    int type = ZEND_INI_PERDIR;
    if ( '\001' == *pKey ) {
        ++pKey;
        if ( *pKey == 4 ) {
            type = ZEND_INI_SYSTEM;
        }
        ++pKey;
        --keyLen;
        if (( keyLen == 7 )&&( strncasecmp( pKey, "engine", 6 )== 0 ))
        {
            if ( *pValue == '0' )
                engine = 0;
        }
        else
        {
#if JONJ_MAJOR_VERSION >= 7
            --keyLen;
            psKey = zend_string_init( pKey, keyLen, 1 );
            zend_alter_ini_entry_chars(psKey, 
                             (char *)pValue, valLen,
                             type, JONJ_INI_STAGE_ACTIVATE);
            zend_string_release( psKey );
#else
            zend_alter_ini_entry((char *)pKey, keyLen,
                             (char *)pValue, valLen,
                             type, JONJ_INI_STAGE_ACTIVATE);            
#endif
        }
    }
    return 1;
}

static void user_config_cache_entry_dtor(
#if JONJ_MAJOR_VERSION >= 7
                                         zval
#else
                                         void
#endif
                                               *el)
{
    user_config_cache_entry *entry =
#if JONJ_MAJOR_VERSION >= 7
                                      (user_config_cache_entry *)Z_PTR_P(el)
#else
                                     *(user_config_cache_entry **)el
#endif
    ;
    zend_hash_destroy(&entry->user_config);
    free(entry);
}

static void user_config_cache_init()
{
    zend_hash_init(&user_config_cache, 0, NULL, user_config_cache_entry_dtor, 1);
}

static int pathlen_without_trailing_slash(char *path)
{
    int len = (int)strlen(path);
    while (len > 1 && /* mind "/" as root dir */
           path[len-1] == DEFAULT_SLASH)
    {
        --len;
    }
    return len;
}

static inline char* skip_slash(char *s)
{
    while (*s == DEFAULT_SLASH) {
        ++s;
    }
    return s;
}

/**
 * Walk down the path_stop starting at path_start.
 *
 * If path_start = "/path1" and path_stop = "/path1/path2/path3"
 * the callback will be called 3 times with the next args:
 *
 *   1. "/path1/path2/path3"
 *             ^ end
 *       ^ start
 *   2. "/path1/path2/path3"
 *                   ^ end
 *       ^ start
 *   3. "/path1/path2/path3"
 *                         ^ end
 *       ^ start
 *
 * path_stop has to be a subdir of path_start
 * or to be path_start itself.
 *
 * Both path args have to be absolute.
 * Trailing slashes are allowed.
 * NULL or empty string args are not allowed.
 */
static void walk_down_the_path(char* path_start,
                               char* path_stop,
                               void (*cb)(char* begin,
                                          char* end,
                                          void* data),
                               void* data)
{
    char *pos = path_stop + pathlen_without_trailing_slash(path_start);
    cb(path_stop, pos, data);

    while ((pos = skip_slash(pos))[0]) {
        pos = strchr(pos, DEFAULT_SLASH);
        if (!pos) {
            /* The last token without trailing slash
             */
            cb(path_stop, path_stop + strlen(path_stop), data);
            return;
        }
        cb(path_stop, pos, data);
    }
}

#if JONJ_MAJOR_VERSION > 5 || (JONJ_MAJOR_VERSION == 5 && JONJ_MINOR_VERSION >= 3)

typedef struct {
    char *path;
    uint path_len;
    char *doc_root;
    user_config_cache_entry *entry;
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
    TSRMLS_D ;
#endif
} _lsapi_activate_user_ini_ctx;

typedef int (*fn_activate_user_ini_chain_t)
        (_lsapi_activate_user_ini_ctx *ctx, void* next);

static int lsapi_activate_user_ini_basic_checks(_lsapi_activate_user_ini_ctx *ctx,
                                                void* next)
{
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    if (!PG(user_ini_filename) || !*PG(user_ini_filename)) {
        return SUCCESS;
    }

    /* PATH_TRANSLATED should be defined at this stage */
    ctx->path = SG(request_info).path_translated;
    if (!ctx->path || !*ctx->path) {
        return FAILURE;
    }

    ctx->doc_root = sapi_lsapi_getenv("DOCUMENT_ROOT", 0
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
                    , ctx->TSRMLS_C
#endif
    );
    DEBUG_MESSAGE("doc_root: %s", ctx->doc_root);

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    return rc;
}

static int lsapi_activate_user_ini_mk_path(_lsapi_activate_user_ini_ctx *ctx,
                                           void* next)
{
    char *path;
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    /* Extract dir name from path_translated * and store it in 'path' */
    ctx->path_len = strlen(ctx->path);
    path = ctx->path = estrndup(SG(request_info).path_translated, ctx->path_len);
    if (!path)
        return FAILURE;
    ctx->path_len = zend_dirname(path, ctx->path_len);
    DEBUG_MESSAGE("dirname: %s", ctx->path);

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    efree(path);
    return rc;
}

static int lsapi_activate_user_ini_mk_realpath(_lsapi_activate_user_ini_ctx *ctx,
                                               void* next)
{
    char *real_path;
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    if (!IS_ABSOLUTE_PATH(ctx->path, ctx->path_len)) {
        real_path = tsrm_realpath(ctx->path, NULL
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
                    , ctx->TSRMLS_C
#endif
        );
        if (!real_path) {
            return SUCCESS;
        }
        ctx->path = real_path;
        ctx->path_len = strlen(ctx->path);
        DEBUG_MESSAGE("calculated tsrm realpath: %s", real_path);
    } else {
        DEBUG_MESSAGE("%s is an absolute path", ctx->path);
        real_path = NULL;
    }

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    if (real_path)
        efree(real_path);
    return rc;
}

static int lsapi_activate_user_ini_mk_user_config(_lsapi_activate_user_ini_ctx *ctx,
                                                  void* next)
{
    fn_activate_user_ini_chain_t *fn_next = next;

    /* Find cached config entry: If not found, create one */
#if JONJ_MAJOR_VERSION >= 7
    ctx->entry = zend_hash_str_find_ptr(&user_config_cache, ctx->path, ctx->path_len);
#else
    {
        user_config_cache_entry **entry_pp;
        if (SUCCESS == zend_hash_find(&user_config_cache, ctx->path,
                                            ctx->path_len + 1, (void **) &entry_pp))
            ctx->entry = *entry_pp;
        else
            ctx->entry = NULL;
    }
#endif

    if (ctx->entry) {
        DEBUG_MESSAGE("found entry for %s", ctx->path);
    } else {
        DEBUG_MESSAGE("entry for %s not found, creating new entry", ctx->path);
        ctx->entry = pemalloc(sizeof(user_config_cache_entry), 1);
        ctx->entry->expires = 0;
        zend_hash_init(&ctx->entry->user_config, 0, NULL,
#if JONJ_MAJOR_VERSION <= 5
                       (dtor_func_t)
#endif
                       config_zval_dtor, 1);
#if JONJ_MAJOR_VERSION >= 7
        zend_hash_str_update_ptr(&user_config_cache, ctx->path,
                                            ctx->path_len, ctx->entry);
#else
        zend_hash_update(&user_config_cache, ctx->path, ctx->path_len + 1,
                                &ctx->entry, sizeof(&ctx->entry), NULL);
#endif
    }

    if (*fn_next) {
        return (*fn_next)(ctx, fn_next + 1);
    } else {
        return SUCCESS;
    }
}

static void walk_down_the_path_callback(char* begin,
                                        char* end,
                                        void* data)
{
    _lsapi_activate_user_ini_ctx *ctx = data;
    char tmp = end[0];
    end[0] = 0;
    DEBUG_MESSAGE("parsing %s%c%s", begin, DEFAULT_SLASH, PG(user_ini_filename));
    jonj_parse_user_ini_file(begin, PG(user_ini_filename), &ctx->entry->user_config
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
                    , ctx->TSRMLS_C
#endif
    );
    end[0] = tmp;
}

static int lsapi_activate_user_ini_walk_down_the_path(_lsapi_activate_user_ini_ctx *ctx,
                                                      void* next)
{
    time_t request_time = sapi_get_request_time(
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
                    ctx->TSRMLS_C
#endif
    );
    uint path_len, docroot_len;
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    if (!ctx->entry->expires || request_time > ctx->entry->expires)
    {
        docroot_len = ctx->doc_root && ctx->doc_root[0]
                    ? pathlen_without_trailing_slash(ctx->doc_root)
                    : 0;

        int is_outside_of_docroot = !docroot_len ||
                ctx->path_len < docroot_len ||
                strncmp(ctx->path, ctx->doc_root, docroot_len) != 0;

        if (is_outside_of_docroot) {
            jonj_parse_user_ini_file(ctx->path, PG(user_ini_filename), &ctx->entry->user_config
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
                    , ctx->TSRMLS_C
#endif
            );
        } else {
            walk_down_the_path(ctx->doc_root, ctx->path,
                               &walk_down_the_path_callback, ctx);
        }

        ctx->entry->expires = request_time + PG(user_ini_cache_ttl);
    }

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    return rc;
}

static int lsapi_activate_user_ini_finally(_lsapi_activate_user_ini_ctx *ctx,
                                           void* next)
{
    int rc = SUCCESS;
    fn_activate_user_ini_chain_t *fn_next = next;

    DEBUG_MESSAGE("calling jonj_ini_activate_config()");
    jonj_ini_activate_config(&ctx->entry->user_config, JONJ_INI_PERDIR, JONJ_INI_STAGE_HTACCESS
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
                    , ctx->TSRMLS_C
#endif
    );

    if (*fn_next) {
        rc = (*fn_next)(ctx, fn_next + 1);
    }

    return rc;
}

static int lsapi_activate_user_ini(TSRMLS_D)
{
    _lsapi_activate_user_ini_ctx ctx;
    /**
     * The reason to have this function list stacked
     * is each function now can define a scoped destructor.
     *
     * Passing control via function pointer is a sign of low coupling,
     * which means dependencies between these functions are to be
     * controlled from a single place
     * (here below, by modifying this function list order)
     */
    static const fn_activate_user_ini_chain_t fn_chain[] = {
        &lsapi_activate_user_ini_basic_checks,
        &lsapi_activate_user_ini_mk_path,
        &lsapi_activate_user_ini_mk_realpath,
        &lsapi_activate_user_ini_mk_user_config,
        &lsapi_activate_user_ini_walk_down_the_path,
        &lsapi_activate_user_ini_finally,
        NULL
    };
    
#if defined(ZTS) && JONJ_MAJOR_VERSION < 7
    ctx.TSRMLS_C = TSRMLS_C;
#endif

    return fn_chain[0](&ctx, (fn_activate_user_ini_chain_t*)(fn_chain + 1));
}

#endif

static void override_ini()
{

    LSAPI_ForeachSpecialEnv( alter_ini, NULL );

}


static int processReq( TSRMLS_D )
{
    int ret = 0;
    zend_first_try {

        /* avoid server_context==NULL checks */
        SG(server_context) = (void *) 1;

        engine = 1;
        override_ini();

        if ( engine ) {
            init_request_info( TSRMLS_C );

            if ( lsapi_module_main( source_highlight TSRMLS_CC ) == -1 ) {
                ret = -1;
            }
        } else {
            LSAPI_AppendRespHeader( "status: 403", 11 );
            LSAPI_AppendRespHeader( "content-type: text/html", 23 );
            LSAPI_Write( "Forbidden: JONJ engine is disable.\n", 34 );
        }
    } zend_end_try();
    return ret;
}

static void cli_usage( TSRMLS_D )
{
    static const char * usage =
        "Usage: jonj\n"
        "      jonj -[b|c|n|h|i|q|s|v|?] [<file>] [args...]\n"
        "  Run in LSAPI mode, only '-b', '-s' and '-c' are effective\n"
        "  Run in Command Line Interpreter mode when parameters are specified\n"
        "\n"
        "  -b <address:port>|<port> Bind Path for external LSAPI Server mode\n"
        "  -c <path>|<file> Look for jonj.ini file in this directory\n"
        "  -n    No jonj.ini file will be used\n"
        "  -h    This help\n"
        "  -i    JONJ information\n"
        "  -l    Syntax check\n"
        "  -q    Quiet-mode.  Suppress HTTP Header output.\n"
        "  -s    Display colour syntax highlighted source.\n"
        "  -v    Version number\n"
        "  -?    This help\n"
        "\n"
        "  args...    Arguments passed to script.\n";
    jonj_output_startup();
    jonj_output_activate(TSRMLS_C);
    jonj_printf( "%s", usage );
#ifdef JONJ_OUTPUT_NEWAPI
    jonj_output_end_all(TSRMLS_C);
#else
    jonj_end_ob_buffers(1 TSRMLS_CC);
#endif
}

static int parse_opt( int argc, char * argv[], int *climode,
                        char **jonj_ini_path, char ** jonj_bind )
{
    char ** p = &argv[1];
    char ** argend= &argv[argc];
    int c;
    while (( p < argend )&&(**p == '-' )) {
        c = *((*p)+1);
        ++p;
        switch( c ) {
        case 'b':
            if ( p >= argend ) {
                fprintf( stderr, "TCP or socket address must be specified following '-b' option.\n");
                return -1;
            }
            *jonj_bind = strdup(*p++);
            break;
            
        case 'c':
            if ( p >= argend ) {
                fprintf( stderr, "<path> or <file> must be specified following '-c' option.\n");

                return -1;
            }
            *jonj_ini_path = strdup( *p++ );
            break;
        case 's':
            source_highlight = 1;
            break;
        case 'n':
            ignore_jonj_ini = 1;
            break;
        case '?':
            if ( *((*(p-1))+2) == 's' )
                exit( 99 );
        case 'h':
        case 'i':
        case 'l':
        case 'q':
        case 'v':
        default:
            *climode = 1;
            break;
        }
    }
    if ( p - argv < argc ) {
        *climode = 1;
    }
    return 0;
}

static int cli_main( int argc, char * argv[] )
{

    static const char * ini_defaults[] = {
        "report_zend_debug",    "0",
        "display_errors",       "1",
        "register_argc_argv",   "1",
        "html_errors",          "0",
        "implicit_flush",       "1",
        "output_buffering",     "0",
        "max_execution_time",   "0",
        "max_input_time",       "-1",
        NULL
    };

    const char ** ini;
    char ** p = &argv[1];
    char ** argend= &argv[argc];
    int ret = -1;
    int c;
#if JONJ_MAJOR_VERSION >= 7
	zend_string * psKey; 
#endif
    lsapi_mode = 0;        /* enter CLI mode */

#ifdef JONJ_WIN32
    _fmode = _O_BINARY;            /*sets default for file streams to binary */
    setmode(_fileno(stdin), O_BINARY);    /* make the stdio mode be binary */
    setmode(_fileno(stdout), O_BINARY);   /* make the stdio mode be binary */
    setmode(_fileno(stderr), O_BINARY);   /* make the stdio mode be binary */
#endif

    zend_first_try     {
        SG(server_context) = (void *) 1;

        zend_uv.html_errors = 0; /* tell the engine we're in non-html mode */
        CG(in_compilation) = 0; /* not initialized but needed for several options */
        SG(options) |= SAPI_OPTION_NO_CHDIR;
        
#if JONJ_MAJOR_VERSION < 7
        EG(uninitialized_zval_ptr) = NULL;
#endif
        for( ini = ini_defaults; *ini; ini+=2 ) {
#if JONJ_MAJOR_VERSION >= 7
			psKey = zend_string_init( *ini, strlen( *ini ), 1 );
            zend_alter_ini_entry_chars( psKey, 
                                (char *)*(ini+1), strlen( *(ini+1) ),
                                JONJ_INI_SYSTEM, JONJ_INI_STAGE_ACTIVATE);
            zend_string_release( psKey );
#else
            zend_alter_ini_entry( (char *)*ini, strlen( *ini )+1,
                                (char *)*(ini+1), strlen( *(ini+1) ),
                                JONJ_INI_SYSTEM, JONJ_INI_STAGE_ACTIVATE);
#endif
        }

        while (( p < argend )&&(**p == '-' )) {
            c = *((*p)+1);
            ++p;
            switch( c ) {
            case 'q':
                break;
            case 'i':
                if (jonj_request_startup(TSRMLS_C) != FAILURE) {
                    jonj_print_info(0xFFFFFFFF TSRMLS_CC);
#ifdef JONJ_OUTPUT_NEWAPI
                    jonj_output_end_all(TSRMLS_C);
#else
                    jonj_end_ob_buffers(1 TSRMLS_CC);
#endif
                    jonj_request_shutdown( NULL );
                    ret = 0;
                }
                break;
            case 'v':
                if (jonj_request_startup(TSRMLS_C) != FAILURE) {
#if ZEND_DEBUG
                    jonj_printf("JONJ %s (%s) (built: %s %s) (DEBUG)\nCopyright (c) 1997-2016 The JONJ Group\n%s", JONJ_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#else
                    jonj_printf("JONJ %s (%s) (built: %s %s)\nCopyright (c) 1997-2016 The JONJ Group\n%s", JONJ_VERSION, sapi_module.name, __DATE__, __TIME__, get_zend_version());
#endif
#ifdef JONJ_OUTPUT_NEWAPI
                    jonj_output_end_all(TSRMLS_C);
#else
                    jonj_end_ob_buffers(1 TSRMLS_CC);
#endif
                    jonj_request_shutdown( NULL );
                    ret = 0;
                }
                break;
            case 'c':
                ++p;
            /* fall through */
            case 's':
                break;
            case 'l':
                source_highlight = 2;
                break;
            case 'h':
            case '?':
            default:
                cli_usage(TSRMLS_C);
                ret = 0;
                break;

            }
        }
        if ( ret == -1 ) {
            if ( *p ) {
                zend_file_handle file_handle = {0};

                file_handle.type = ZEND_HANDLE_FP;
                file_handle.handle.fp = VCWD_FOPEN(*p, "rb");

                if ( file_handle.handle.fp ) {
                    script_filename = *p;
                    jonj_self = *p;

                    SG(request_info).path_translated = estrdup(*p);
                    SG(request_info).argc = argc - (p - argv);
                    SG(request_info).argv = p;

                    if (jonj_request_startup(TSRMLS_C) == FAILURE ) {
                        fclose( file_handle.handle.fp );
                        ret = 2;
                    } else {
                        if (source_highlight == 1) {
                            zend_syntax_highlighter_ini syntax_highlighter_ini;
                    
                            jonj_get_highlight_struct(&syntax_highlighter_ini);
                            highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC);
                        } else if (source_highlight == 2) {
                            file_handle.filename = *p;
                            file_handle.free_filename = 0;
                            file_handle.opened_path = NULL;
                            ret = jonj_lint_script(&file_handle TSRMLS_CC);
                            if (ret==SUCCESS) {
                                zend_printf("No syntax errors detected in %s\n", file_handle.filename);
                            } else {
                                zend_printf("Errors parsing %s\n", file_handle.filename);
                            }
                            
                        } else {
                            file_handle.filename = *p;
                            file_handle.free_filename = 0;
                            file_handle.opened_path = NULL;

                            jonj_execute_script(&file_handle TSRMLS_CC);
                            ret = EG(exit_status);
                       }

                        jonj_request_shutdown( NULL );
                    }
                } else {
                    jonj_printf("Could not open input file: %s.\n", *p);
                }
            } else {
                cli_usage(TSRMLS_C);
            }
        }

    }zend_end_try();

    jonj_module_shutdown(TSRMLS_C);

#ifdef ZTS
    tsrm_shutdown();
#endif
    return ret;
}

static int s_stop;
void litespeed_cleanup(int signal)
{
    s_stop = signal;
}


void start_children( int children )
{
    struct sigaction act, old_term, old_quit, old_int, old_usr1;
    int running = 0;
    int status;
    pid_t pid;

    /* Create a process group */
    setsid();

    /* Set up handler to kill children upon exit */
    act.sa_flags = 0;
    act.sa_handler = litespeed_cleanup;
    if( sigaction( SIGTERM, &act, &old_term ) ||
        sigaction( SIGINT,  &act, &old_int  ) ||
        sigaction( SIGUSR1, &act, &old_usr1 ) ||
        sigaction( SIGQUIT, &act, &old_quit )) {
        perror( "Can't set signals" );
        exit( 1 );
    }
    s_stop = 0;
    while( 1 ) {
        while((!s_stop )&&( running < children )) {
            pid = fork();
            switch( pid ) {
            case 0: /* children process */

                /* don't catch our signals */
                sigaction( SIGTERM, &old_term, 0 );
                sigaction( SIGQUIT, &old_quit, 0 );
                sigaction( SIGINT,  &old_int,  0 );
                sigaction( SIGUSR1, &old_usr1, 0 );
                return ;
            case -1:
                perror( "jonj (pre-forking)" );
                exit( 1 );
                break;
            default: /* parent process */
                running++;
                break;
            }
        } 
        if ( s_stop ) {
            break;
        }
        pid = wait( &status );
        running--;
    }
    kill( -getpgrp(), SIGUSR1 );
    exit( 0 );
}

void setArgv0( int argc, char * argv[] )
{
    char * p;
    int i;
    argv0 = argv[0] + strlen( argv[0] );
    p = argv0;
    while(( p > argv[0] )&&( p[-1] != '/'))
        --p;
    if ( p > argv[0] )
    {
        memmove( argv[0], p, argv0 - p );
        memset( argv[0] + ( argv0 - p ), 0, p - argv[0] );
        argv0 = argv[0] + (argv0 - p);
    }
    for( i = 1; i < argc; ++i )
    {
        memset( argv[i], 0, strlen( argv[i] ) );
    }
}

#include <fcntl.h>
int main( int argc, char * argv[] )
{
    int ret;
    int bindFd;

    char * jonj_ini_path = NULL;
    char * jonj_bind     = NULL;
    int n;
    int climode = 0;
    struct timeval tv_req_begin;
    struct timeval tv_req_end;
    int slow_script_msec = 0;
    char time_buf[40];
    
#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
    signal(SIGPIPE, SIG_IGN);
#endif
#endif

#ifdef ZTS
    tsrm_startup(1, 1, 0, NULL);
#endif

    if (argc > 1 ) {
        if ( parse_opt( argc, argv, &climode, 
                &jonj_ini_path, &jonj_bind ) == -1 ) {
            return 1;
        }
    }
    if ( climode ) {
        lsapi_sapi_module.jonjinfo_as_text = 1;
    } else {
        setArgv0(argc, argv );
    }

    sapi_startup(&lsapi_sapi_module);

#ifdef ZTS
    compiler_globals = ts_resource(compiler_globals_id);
    executor_globals = ts_resource(executor_globals_id);
    core_globals = ts_resource(core_globals_id);
    sapi_globals = ts_resource(sapi_globals_id);
    tsrm_ls = ts_resource(0);

    SG(request_info).path_translated = NULL;
#endif

    lsapi_sapi_module.executable_location = argv[0];

    /* Initialize from environment variables before processing command-line
     * options: the latter override the former.
     */
    init_sapi_from_env(&lsapi_sapi_module);
    
    if ( ignore_jonj_ini )
        lsapi_sapi_module.jonj_ini_ignore = 1;

    if ( jonj_ini_path ) {
        lsapi_sapi_module.jonj_ini_path_override = jonj_ini_path;
    }


    lsapi_sapi_module.ini_defaults = sapi_lsapi_ini_defaults;

    if (jonj_module_startup(&lsapi_sapi_module, &litespeed_module_entry, 1) == FAILURE) {
#ifdef ZTS
        tsrm_shutdown();
#endif
        return FAILURE;
    }

    if ( climode ) {
        return cli_main(argc, argv);
    }

    if ( jonj_bind ) {
        bindFd = LSAPI_CreateListenSock( jonj_bind, 10 );
        if ( bindFd == -1 ) {
            fprintf( stderr,
                     "Failed to bind socket [%s]: %s\n", jonj_bind, strerror( errno ) );
            exit( 2 );
        }
        if ( bindFd != 0 ) {
            dup2( bindFd, 0 );
            close( bindFd );
        }
    }

    LSAPI_Init();
   
    LSAPI_Init_Env_Parameters( NULL );
    lsapi_mode = 1; 

    slow_script_msec = LSAPI_Get_Slow_Req_Msecs();

    if ( jonj_bind ) {
        LSAPI_No_Check_ppid();
        free( jonj_bind );
        jonj_bind = NULL;
    }

    while( LSAPI_Prefork_Accept_r( &g_req ) >= 0 ) {
        if ( slow_script_msec ) {
            gettimeofday( &tv_req_begin, NULL );
        }
        ret = processReq(TSRMLS_C);
        if ( slow_script_msec ) {
            gettimeofday( &tv_req_end, NULL );
            n = ((long) tv_req_end.tv_sec - tv_req_begin.tv_sec ) * 1000 
                + (tv_req_end.tv_usec - tv_req_begin.tv_usec) / 1000;
            if ( n > slow_script_msec )
            {
                strftime( time_buf, 30, "%d/%b/%Y:%H:%M:%S", localtime( &tv_req_end.tv_sec ) );
                fprintf( stderr, "[%s] Slow JONJ script: %d ms\n  URL: %s %s\n  Query String: %s\n  Script: %s\n",
                         time_buf, n,  LSAPI_GetRequestMethod(), 
                         LSAPI_GetScriptName(), LSAPI_GetQueryString(),
                         LSAPI_GetScriptFileName() );

            }
        }
        LSAPI_Finish();
        if ( ret ) {
            break;
        }
    }
    jonj_module_shutdown(TSRMLS_C);

#ifdef ZTS
    tsrm_shutdown();
#endif
    return ret;
}


/*   LiteSpeed JONJ module starts here */

#if JONJ_MAJOR_VERSION > 4

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_litespeed__void, 0)
ZEND_END_ARG_INFO()
/* }}} */

#else
#define arginfo_litespeed__void NULL
#endif

JONJ_FUNCTION(litespeed_request_headers);
JONJ_FUNCTION(litespeed_response_headers);
JONJ_FUNCTION(apache_get_modules);

JONJ_MINFO_FUNCTION(litespeed);


zend_function_entry litespeed_functions[] = {
    JONJ_FE(litespeed_request_headers,   arginfo_litespeed__void)
    JONJ_FE(litespeed_response_headers,  arginfo_litespeed__void)
    JONJ_FE(apache_get_modules,          arginfo_litespeed__void)
    JONJ_FALIAS(getallheaders,           litespeed_request_headers,  arginfo_litespeed__void)
    JONJ_FALIAS(apache_request_headers,  litespeed_request_headers,  arginfo_litespeed__void)
    JONJ_FALIAS(apache_response_headers, litespeed_response_headers, arginfo_litespeed__void)
    {NULL, NULL, NULL}
};

static JONJ_MINIT_FUNCTION(litespeed)
{
    user_config_cache_init();

    const char *p = getenv("LSJONJ_ENABLE_USER_INI");
    if (p && 0 == strcasecmp(p, "on"))
        parse_user_ini = 1;

    /* REGISTER_INI_ENTRIES(); */
    return SUCCESS;
}


static JONJ_MSHUTDOWN_FUNCTION(litespeed)
{
    zend_hash_destroy(&user_config_cache);

    /* UNREGISTER_INI_ENTRIES(); */
    return SUCCESS;
}

zend_module_entry litespeed_module_entry = {
    STANDARD_MODULE_HEADER,
    "litespeed",
    litespeed_functions,
    JONJ_MINIT(litespeed),
    JONJ_MSHUTDOWN(litespeed),
    NULL,
    NULL,
    NULL,
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

static int add_associate_array( const char * pKey, int keyLen, const char * pValue, int valLen,
                         void * arg )
{
#if JONJ_MAJOR_VERSION < 7
    add_assoc_string_ex((zval *)arg, (char *)pKey, keyLen+1, (char *)pValue, 1);
#else
    add_assoc_string_ex((zval *)arg, (char *)pKey, keyLen, (char *)pValue);
#endif        
    return 1;
}


/* {{{ proto array litespeed_request_headers(void)
   Fetch all HTTP request headers */
JONJ_FUNCTION(litespeed_request_headers)
{
    /* TODO: */
    if (ZEND_NUM_ARGS() > 0) {
        WRONG_PARAM_COUNT;
    }
    array_init(return_value);

    LSAPI_ForeachOrgHeader( add_associate_array, return_value );

}
/* }}} */



/* {{{ proto array litespeed_response_headers(void)
   Fetch all HTTP response headers */
JONJ_FUNCTION(litespeed_response_headers)
{
    sapi_header_struct  *h;
    zend_llist_position pos;
    char *       p;
    int          len;
    char         headerBuf[SAPI_LSAPI_MAX_HEADER_LENGTH];

    if (ZEND_NUM_ARGS() > 0) {
        WRONG_PARAM_COUNT;
    }

    if (!&SG(sapi_headers).headers) {
        RETURN_FALSE;
    }
    array_init(return_value);

    h = zend_llist_get_first_ex(&SG(sapi_headers).headers, &pos);
    while (h) {
        if ( h->header_len > 0 ) {
            p = strchr( h->header, ':' );
            len = p - h->header;
            if (( p )&&( len > 0 )) {
                memmove( headerBuf, h->header, len );
                while( len > 0 && (isspace( headerBuf[len-1])) ) {
                    --len;
                }
                headerBuf[len] = 0;
                if ( len ) {
                    while( isspace(*++p));
#if JONJ_MAJOR_VERSION < 7
                    add_assoc_string_ex(return_value, headerBuf, len+1, p, 1);
#else
                    add_assoc_string_ex(return_value, headerBuf, len, p);
#endif        
                }
            }
        }
        h = zend_llist_get_next_ex(&SG(sapi_headers).headers, &pos);
    }  
}

/* }}} */


/* {{{ proto array apache_get_modules(void)
   Fetch all loaded module names  */
JONJ_FUNCTION(apache_get_modules)
{
    static const char * mod_names[] = 
    {
        "mod_rewrite", "mod_mime", "mod_headers", "mod_expires", "mod_auth_basic", NULL
    };
    const char **name = mod_names;
    /* TODO: */
    if (ZEND_NUM_ARGS() > 0) {
        WRONG_PARAM_COUNT;
    }
    array_init(return_value);
    while( *name )
    {
        add_next_index_string(return_value, *name 
#if JONJ_MAJOR_VERSION < 7
                                        , 1
#endif        
        );
        ++name;
    }
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */


