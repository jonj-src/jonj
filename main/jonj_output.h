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
   | Author: Michael Wallner <mike@jonj.tk>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_OUTPUT_H
#define JONJ_OUTPUT_H

#define JONJ_OUTPUT_NEWAPI 1

/* handler ops */
#define JONJ_OUTPUT_HANDLER_WRITE	0x00	/* standard passthru */
#define JONJ_OUTPUT_HANDLER_START	0x01	/* start */
#define JONJ_OUTPUT_HANDLER_CLEAN	0x02	/* restart */
#define JONJ_OUTPUT_HANDLER_FLUSH	0x04	/* pass along as much as possible */
#define JONJ_OUTPUT_HANDLER_FINAL	0x08	/* finalize */
#define JONJ_OUTPUT_HANDLER_CONT		JONJ_OUTPUT_HANDLER_WRITE
#define JONJ_OUTPUT_HANDLER_END		JONJ_OUTPUT_HANDLER_FINAL

/* handler types */
#define JONJ_OUTPUT_HANDLER_INTERNAL		0x0000
#define JONJ_OUTPUT_HANDLER_USER			0x0001

/* handler ability flags */
#define JONJ_OUTPUT_HANDLER_CLEANABLE	0x0010
#define JONJ_OUTPUT_HANDLER_FLUSHABLE	0x0020
#define JONJ_OUTPUT_HANDLER_REMOVABLE	0x0040
#define JONJ_OUTPUT_HANDLER_STDFLAGS		0x0070

/* handler status flags */
#define JONJ_OUTPUT_HANDLER_STARTED		0x1000
#define JONJ_OUTPUT_HANDLER_DISABLED		0x2000
#define JONJ_OUTPUT_HANDLER_PROCESSED	0x4000

/* handler op return values */
typedef enum _jonj_output_handler_status_t {
	JONJ_OUTPUT_HANDLER_FAILURE,
	JONJ_OUTPUT_HANDLER_SUCCESS,
	JONJ_OUTPUT_HANDLER_NO_DATA
} jonj_output_handler_status_t;

/* jonj_output_stack_pop() flags */
#define JONJ_OUTPUT_POP_TRY			0x000
#define JONJ_OUTPUT_POP_FORCE		0x001
#define JONJ_OUTPUT_POP_DISCARD		0x010
#define JONJ_OUTPUT_POP_SILENT		0x100

/* real global flags */
#define JONJ_OUTPUT_IMPLICITFLUSH		0x01
#define JONJ_OUTPUT_DISABLED				0x02
#define JONJ_OUTPUT_WRITTEN				0x04
#define JONJ_OUTPUT_SENT					0x08
/* supplementary flags for jonj_output_get_status() */
#define JONJ_OUTPUT_ACTIVE				0x10
#define JONJ_OUTPUT_LOCKED				0x20
/* output layer is ready to use */
#define JONJ_OUTPUT_ACTIVATED		0x100000

/* handler hooks */
typedef enum _jonj_output_handler_hook_t {
	JONJ_OUTPUT_HANDLER_HOOK_GET_OPAQ,
	JONJ_OUTPUT_HANDLER_HOOK_GET_FLAGS,
	JONJ_OUTPUT_HANDLER_HOOK_GET_LEVEL,
	JONJ_OUTPUT_HANDLER_HOOK_IMMUTABLE,
	JONJ_OUTPUT_HANDLER_HOOK_DISABLE,
	/* unused */
	JONJ_OUTPUT_HANDLER_HOOK_LAST
} jonj_output_handler_hook_t;

#define JONJ_OUTPUT_HANDLER_INITBUF_SIZE(s) \
( ((s) > 1) ? \
	(s) + JONJ_OUTPUT_HANDLER_ALIGNTO_SIZE - ((s) % (JONJ_OUTPUT_HANDLER_ALIGNTO_SIZE)) : \
	JONJ_OUTPUT_HANDLER_DEFAULT_SIZE \
)
#define JONJ_OUTPUT_HANDLER_ALIGNTO_SIZE		0x1000
#define JONJ_OUTPUT_HANDLER_DEFAULT_SIZE		0x4000

typedef struct _jonj_output_buffer {
	char *data;
	size_t size;
	size_t used;
	uint free:1;
	uint _res:31;
} jonj_output_buffer;

typedef struct _jonj_output_context {
	int op;
	jonj_output_buffer in;
	jonj_output_buffer out;
#ifdef ZTS
	void ***tsrm_ls;
#endif
} jonj_output_context;

#define JONJ_OUTPUT_TSRMLS(ctx) TSRMLS_FETCH_FROM_CTX((ctx)->tsrm_ls)

/* old-style, stateless callback */
typedef void (*jonj_output_handler_func_t)(char *output, uint output_len, char **handled_output, uint *handled_output_len, int mode TSRMLS_DC);
/* new-style, opaque context callback */
typedef int (*jonj_output_handler_context_func_t)(void **handler_context, jonj_output_context *output_context);
/* output handler context dtor */
typedef void (*jonj_output_handler_context_dtor_t)(void *opaq TSRMLS_DC);
/* conflict check callback */
typedef int (*jonj_output_handler_conflict_check_t)(const char *handler_name, size_t handler_name_len TSRMLS_DC);
/* ctor for aliases */
typedef struct _jonj_output_handler *(*jonj_output_handler_alias_ctor_t)(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags TSRMLS_DC);

typedef struct _jonj_output_handler_user_func_t {
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *zoh;
} jonj_output_handler_user_func_t;

typedef struct _jonj_output_handler {
	char *name;
	size_t name_len;
	int flags;
	int level;
	size_t size;
	jonj_output_buffer buffer;
	
	void *opaq;
	void (*dtor)(void *opaq TSRMLS_DC);
	
	union {
		jonj_output_handler_user_func_t *user;
		jonj_output_handler_context_func_t internal;
	} func;
} jonj_output_handler;

ZEND_BEGIN_MODULE_GLOBALS(output)
	int flags;
	zend_stack handlers;
	jonj_output_handler *active;
	jonj_output_handler *running;
	const char *output_start_filename;
	int output_start_lineno;
ZEND_END_MODULE_GLOBALS(output)

JONJAPI ZEND_EXTERN_MODULE_GLOBALS(output);

/* there should not be a need to use OG() from outside of output.c */
#ifdef ZTS
# define OG(v) TSRMG(output_globals_id, zend_output_globals *, v)
#else
# define OG(v) (output_globals.v)
#endif

/* convenience macros */
#define JONJWRITE(str, str_len)		jonj_output_write((str), (str_len) TSRMLS_CC)
#define JONJWRITE_H(str, str_len)	jonj_output_write_unbuffered((str), (str_len) TSRMLS_CC)

#define PUTC(c)						(jonj_output_write(&(c), 1 TSRMLS_CC), (c))
#define PUTC_H(c)					(jonj_output_write_unbuffered(&(c), 1 TSRMLS_CC), (c))

#define PUTS(str)					do {				\
	const char *__str = (str);							\
	jonj_output_write(__str, strlen(__str) TSRMLS_CC);	\
} while (0)
#define PUTS_H(str)					do {							\
	const char *__str = (str);										\
	jonj_output_write_unbuffered(__str, strlen(__str) TSRMLS_CC);	\
} while (0)


BEGIN_EXTERN_C()

extern const char jonj_output_default_handler_name[sizeof("default output handler")];
extern const char jonj_output_devnull_handler_name[sizeof("null output handler")];

#define jonj_output_tearup() \
	jonj_output_startup(); \
	jonj_output_activate(TSRMLS_C)
#define jonj_output_teardown() \
	jonj_output_end_all(TSRMLS_C); \
	jonj_output_deactivate(TSRMLS_C); \
	jonj_output_shutdown()

/* MINIT */
JONJAPI void jonj_output_startup(void);
/* MSHUTDOWN */
JONJAPI void jonj_output_shutdown(void);

JONJAPI void jonj_output_register_constants(TSRMLS_D);

/* RINIT */
JONJAPI int jonj_output_activate(TSRMLS_D);
/* RSHUTDOWN */
JONJAPI void jonj_output_deactivate(TSRMLS_D);

JONJAPI void jonj_output_set_status(int status TSRMLS_DC);
JONJAPI int jonj_output_get_status(TSRMLS_D);
JONJAPI void jonj_output_set_implicit_flush(int flush TSRMLS_DC);
JONJAPI const char *jonj_output_get_start_filename(TSRMLS_D);
JONJAPI int jonj_output_get_start_lineno(TSRMLS_D);

JONJAPI int jonj_output_write_unbuffered(const char *str, size_t len TSRMLS_DC);
JONJAPI int jonj_output_write(const char *str, size_t len TSRMLS_DC);

JONJAPI int jonj_output_flush(TSRMLS_D);
JONJAPI void jonj_output_flush_all(TSRMLS_D);
JONJAPI int jonj_output_clean(TSRMLS_D);
JONJAPI void jonj_output_clean_all(TSRMLS_D);
JONJAPI int jonj_output_end(TSRMLS_D);
JONJAPI void jonj_output_end_all(TSRMLS_D);
JONJAPI int jonj_output_discard(TSRMLS_D);
JONJAPI void jonj_output_discard_all(TSRMLS_D);

JONJAPI int jonj_output_get_contents(zval *p TSRMLS_DC);
JONJAPI int jonj_output_get_length(zval *p TSRMLS_DC);
JONJAPI int jonj_output_get_level(TSRMLS_D);
JONJAPI jonj_output_handler* jonj_output_get_active_handler(TSRMLS_D);

JONJAPI int jonj_output_start_default(TSRMLS_D);
JONJAPI int jonj_output_start_devnull(TSRMLS_D);

JONJAPI int jonj_output_start_user(zval *output_handler, size_t chunk_size, int flags TSRMLS_DC);
JONJAPI int jonj_output_start_internal(const char *name, size_t name_len, jonj_output_handler_func_t output_handler, size_t chunk_size, int flags TSRMLS_DC);

JONJAPI jonj_output_handler *jonj_output_handler_create_user(zval *handler, size_t chunk_size, int flags TSRMLS_DC);
JONJAPI jonj_output_handler *jonj_output_handler_create_internal(const char *name, size_t name_len, jonj_output_handler_context_func_t handler, size_t chunk_size, int flags TSRMLS_DC);

JONJAPI void jonj_output_handler_set_context(jonj_output_handler *handler, void *opaq, void (*dtor)(void* TSRMLS_DC) TSRMLS_DC);
JONJAPI int jonj_output_handler_start(jonj_output_handler *handler TSRMLS_DC);
JONJAPI int jonj_output_handler_started(const char *name, size_t name_len TSRMLS_DC);
JONJAPI int jonj_output_handler_hook(jonj_output_handler_hook_t type, void *arg TSRMLS_DC);
JONJAPI void jonj_output_handler_dtor(jonj_output_handler *handler TSRMLS_DC);
JONJAPI void jonj_output_handler_free(jonj_output_handler **handler TSRMLS_DC);

JONJAPI int jonj_output_handler_conflict(const char *handler_new, size_t handler_new_len, const char *handler_set, size_t handler_set_len TSRMLS_DC);
JONJAPI int jonj_output_handler_conflict_register(const char *handler_name, size_t handler_name_len, jonj_output_handler_conflict_check_t check_func TSRMLS_DC);
JONJAPI int jonj_output_handler_reverse_conflict_register(const char *handler_name, size_t handler_name_len, jonj_output_handler_conflict_check_t check_func TSRMLS_DC);

JONJAPI jonj_output_handler_alias_ctor_t *jonj_output_handler_alias(const char *handler_name, size_t handler_name_len TSRMLS_DC);
JONJAPI int jonj_output_handler_alias_register(const char *handler_name, size_t handler_name_len, jonj_output_handler_alias_ctor_t func TSRMLS_DC);

END_EXTERN_C()


JONJ_FUNCTION(ob_start);
JONJ_FUNCTION(ob_flush);
JONJ_FUNCTION(ob_clean);
JONJ_FUNCTION(ob_end_flush);
JONJ_FUNCTION(ob_end_clean);
JONJ_FUNCTION(ob_get_flush);
JONJ_FUNCTION(ob_get_clean);
JONJ_FUNCTION(ob_get_contents);
JONJ_FUNCTION(ob_get_length);
JONJ_FUNCTION(ob_get_level);
JONJ_FUNCTION(ob_get_status);
JONJ_FUNCTION(ob_implicit_flush);
JONJ_FUNCTION(ob_list_handlers);

JONJ_FUNCTION(output_add_rewrite_var);
JONJ_FUNCTION(output_reset_rewrite_vars);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
