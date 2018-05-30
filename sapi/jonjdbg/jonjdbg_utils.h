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
   | Authors: Felipe Pena <felipe@jonj.tk>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@jonj.tk>                                |
   +----------------------------------------------------------------------+
*/

#ifndef JONJDBG_UTILS_H
#define JONJDBG_UTILS_H

/**
 * Input scan functions
 */
JONJDBG_API int jonjdbg_is_numeric(const char*);
JONJDBG_API int jonjdbg_is_empty(const char*);
JONJDBG_API int jonjdbg_is_addr(const char*);
JONJDBG_API int jonjdbg_is_class_method(const char*, size_t, char**, char**);
JONJDBG_API const char *jonjdbg_current_file(TSRMLS_D);
JONJDBG_API char *jonjdbg_resolve_path(const char* TSRMLS_DC);
JONJDBG_API char *jonjdbg_trim(const char*, size_t, size_t*);
JONJDBG_API const zend_function *jonjdbg_get_function(const char*, const char* TSRMLS_DC);

/**
 * Error/notice/formatting helpers
 */
enum {
	P_ERROR  = 1,
	P_NOTICE,
	P_WRITELN,
	P_WRITE,
	P_LOG
};

#ifdef ZTS
JONJDBG_API int jonjdbg_print(int TSRMLS_DC, FILE*, const char*, ...) JONJ_ATTRIBUTE_FORMAT(printf, 4, 5);
#else
JONJDBG_API int jonjdbg_print(int TSRMLS_DC, FILE*, const char*, ...) JONJ_ATTRIBUTE_FORMAT(printf, 3, 4);
#endif

JONJDBG_API int jonjdbg_rlog(FILE *stream, const char *fmt, ...);

#define jonjdbg_error(fmt, ...)              jonjdbg_print(P_ERROR   TSRMLS_CC, JONJDBG_G(io)[JONJDBG_STDOUT], fmt, ##__VA_ARGS__)
#define jonjdbg_notice(fmt, ...)             jonjdbg_print(P_NOTICE  TSRMLS_CC, JONJDBG_G(io)[JONJDBG_STDOUT], fmt, ##__VA_ARGS__)
#define jonjdbg_writeln(fmt, ...)            jonjdbg_print(P_WRITELN TSRMLS_CC, JONJDBG_G(io)[JONJDBG_STDOUT], fmt, ##__VA_ARGS__)
#define jonjdbg_write(fmt, ...)              jonjdbg_print(P_WRITE   TSRMLS_CC, JONJDBG_G(io)[JONJDBG_STDOUT], fmt, ##__VA_ARGS__)
#define jonjdbg_log(fmt, ...)                jonjdbg_print(P_LOG     TSRMLS_CC, JONJDBG_G(io)[JONJDBG_STDOUT], fmt, ##__VA_ARGS__)

#define jonjdbg_error_ex(out, fmt, ...)      jonjdbg_print(P_ERROR   TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define jonjdbg_notice_ex(out, fmt, ...)     jonjdbg_print(P_NOTICE  TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define jonjdbg_writeln_ex(out, fmt, ...)    jonjdbg_print(P_WRITELN TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define jonjdbg_write_ex(out, fmt, ...)      jonjdbg_print(P_WRITE   TSRMLS_CC, out, fmt, ##__VA_ARGS__)
#define jonjdbg_log_ex(out, fmt, ...)        jonjdbg_print(P_LOG     TSRMLS_CC, out, fmt, ##__VA_ARGS__)

#if JONJDBG_DEBUG
#	define jonjdbg_debug(fmt, ...) jonjdbg_print(P_LOG   TSRMLS_CC, JONJDBG_G(io)[JONJDBG_STDERR], fmt, ##__VA_ARGS__)
#else
#	define jonjdbg_debug(fmt, ...)
#endif

/* {{{ For writing blank lines */
#define EMPTY NULL /* }}} */

/* {{{ For prompt lines */
#define PROMPT "jonjdbg>" /* }}} */

/* {{{ For separation */
#define SEPARATE "------------------------------------------------" /* }}} */

/* {{{ Color Management */
#define JONJDBG_COLOR_LEN 12
#define JONJDBG_COLOR_D(color, code) \
	{color, sizeof(color)-1, code}
#define JONJDBG_COLOR_END \
	{NULL, 0L, {0}}
#define JONJDBG_ELEMENT_LEN 3
#define JONJDBG_ELEMENT_D(name, id) \
	{name, sizeof(name)-1, id}
#define JONJDBG_ELEMENT_END \
	{NULL, 0L, 0}

#define JONJDBG_COLOR_INVALID	-1
#define JONJDBG_COLOR_PROMPT 	 0
#define JONJDBG_COLOR_ERROR		 1
#define JONJDBG_COLOR_NOTICE		 2
#define JONJDBG_COLORS			 3

typedef struct _jonjdbg_color_t {
	char       *name;
	size_t      name_length;
	const char  code[JONJDBG_COLOR_LEN];
} jonjdbg_color_t;

typedef struct _jonjdbg_element_t {
	char		*name;
	size_t		name_length;
	int			id;
} jonjdbg_element_t;

JONJDBG_API const jonjdbg_color_t *jonjdbg_get_color(const char *name, size_t name_length TSRMLS_DC);
JONJDBG_API void jonjdbg_set_color(int element, const jonjdbg_color_t *color TSRMLS_DC);
JONJDBG_API void jonjdbg_set_color_ex(int element, const char *name, size_t name_length TSRMLS_DC);
JONJDBG_API const jonjdbg_color_t* jonjdbg_get_colors(TSRMLS_D); 
JONJDBG_API int jonjdbg_get_element(const char *name, size_t len TSRMLS_DC); /* }}} */

/* {{{ Prompt Management */
JONJDBG_API void jonjdbg_set_prompt(const char* TSRMLS_DC);
JONJDBG_API const char *jonjdbg_get_prompt(TSRMLS_D); /* }}} */

/* {{{ Console Width */
JONJDBG_API int jonjdbg_get_terminal_width(TSRMLS_D); /* }}} */

int jonjdbg_rebuild_symtable(TSRMLS_D);

#if JONJ_VERSION_ID < 50500
/* copy from zend_hash.c JONJ 5.5 for 5.4 compatibility */
static void zend_hash_get_current_key_zval_ex(const HashTable *ht, zval *key, HashPosition *pos) {
	Bucket *p;

	p = pos ? (*pos) : ht->pInternalPointer;

	if (!p) {
		Z_TYPE_P(key) = IS_NULL;
	} else if (p->nKeyLength) {
		Z_TYPE_P(key) = IS_STRING;
		Z_STRVAL_P(key) = IS_INTERNED(p->arKey) ? (char*)p->arKey : estrndup(p->arKey, p->nKeyLength - 1);
		Z_STRLEN_P(key) = p->nKeyLength - 1;
	} else {
		Z_TYPE_P(key) = IS_LONG;
		Z_LVAL_P(key) = p->h;
	}
}
#endif

#endif /* JONJDBG_UTILS_H */
