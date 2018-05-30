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

#ifndef JONJDBG_BP_H
#define JONJDBG_BP_H

/* {{{ */
typedef struct _zend_op *jonjdbg_opline_ptr_t; /* }}} */

/* {{{ breakpoint base structure */
#define jonjdbg_breakbase(name) \
	int         id; \
	zend_uchar  type; \
	zend_ulong  hits; \
	zend_bool   disabled; \
	const char *name /* }}} */

/* {{{ breakpoint base */
typedef struct _jonjdbg_breakbase_t {
	jonjdbg_breakbase(name);
} jonjdbg_breakbase_t; /* }}} */

/**
 * Breakpoint file-based representation
 */
typedef struct _jonjdbg_breakfile_t {
	jonjdbg_breakbase(filename);
	long        line;
} jonjdbg_breakfile_t;

/**
 * Breakpoint symbol-based representation
 */
typedef struct _jonjdbg_breaksymbol_t {
	jonjdbg_breakbase(symbol);
} jonjdbg_breaksymbol_t;

/**
 * Breakpoint method based representation
 */
typedef struct _jonjdbg_breakmethod_t {
	jonjdbg_breakbase(class_name);
	size_t      class_len;
	const char *func_name;
	size_t      func_len;
} jonjdbg_breakmethod_t;

/**
 * Breakpoint opline num based representation
 */
typedef struct _jonjdbg_breakopline_t {
	jonjdbg_breakbase(func_name);
	size_t      func_len;
	const char *class_name;
	size_t      class_len;
	zend_ulong  opline_num;
	zend_ulong  opline;
} jonjdbg_breakopline_t;

/**
 * Breakpoint opline based representation
 */
typedef struct _jonjdbg_breakline_t {
	jonjdbg_breakbase(name);
	zend_ulong opline;
	jonjdbg_breakopline_t *base;
} jonjdbg_breakline_t;

/**
 * Breakpoint opcode based representation
 */
typedef struct _jonjdbg_breakop_t {
	jonjdbg_breakbase(name);
	zend_ulong hash;
} jonjdbg_breakop_t;

/**
 * Breakpoint condition based representation
 */
typedef struct _jonjdbg_breakcond_t {
	jonjdbg_breakbase(code);
	size_t          code_len;
	zend_bool       paramed;
	jonjdbg_param_t  param;
	zend_ulong      hash;
	zend_op_array  *ops;
} jonjdbg_breakcond_t;

/* {{{ Opline breaks API */
JONJDBG_API void jonjdbg_resolve_op_array_breaks(zend_op_array *op_array TSRMLS_DC);
JONJDBG_API int jonjdbg_resolve_op_array_break(jonjdbg_breakopline_t *brake, zend_op_array *op_array TSRMLS_DC);
JONJDBG_API int jonjdbg_resolve_opline_break(jonjdbg_breakopline_t *new_break TSRMLS_DC); /* }}} */

/* {{{ Breakpoint Creation API */
JONJDBG_API void jonjdbg_set_breakpoint_file(const char* filename, long lineno TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_symbol(const char* func_name, size_t func_name_len TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_method(const char* class_name, const char* func_name TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_opcode(const char* opname, size_t opname_len TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_opline(zend_ulong opline TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_opline_ex(jonjdbg_opline_ptr_t opline TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_method_opline(const char *class, const char *method, zend_ulong opline TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_function_opline(const char *function, zend_ulong opline TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_file_opline(const char *file, zend_ulong opline TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_expression(const char* expression, size_t expression_len TSRMLS_DC);
JONJDBG_API void jonjdbg_set_breakpoint_at(const jonjdbg_param_t *param TSRMLS_DC); /* }}} */

/* {{{ Breakpoint Detection API */
JONJDBG_API jonjdbg_breakbase_t* jonjdbg_find_breakpoint(zend_execute_data* TSRMLS_DC); /* }}} */

/* {{{ Misc Breakpoint API */
JONJDBG_API void jonjdbg_hit_breakpoint(jonjdbg_breakbase_t* brake, zend_bool output TSRMLS_DC);
JONJDBG_API void jonjdbg_print_breakpoints(zend_ulong type TSRMLS_DC);
JONJDBG_API void jonjdbg_print_breakpoint(jonjdbg_breakbase_t* brake TSRMLS_DC);
JONJDBG_API void jonjdbg_reset_breakpoints(TSRMLS_D);
JONJDBG_API void jonjdbg_clear_breakpoints(TSRMLS_D);
JONJDBG_API void jonjdbg_delete_breakpoint(zend_ulong num TSRMLS_DC);
JONJDBG_API void jonjdbg_enable_breakpoints(TSRMLS_D);
JONJDBG_API void jonjdbg_enable_breakpoint(zend_ulong id TSRMLS_DC);
JONJDBG_API void jonjdbg_disable_breakpoint(zend_ulong id TSRMLS_DC);
JONJDBG_API void jonjdbg_disable_breakpoints(TSRMLS_D); /* }}} */

/* {{{ Breakbase API */
JONJDBG_API jonjdbg_breakbase_t *jonjdbg_find_breakbase(zend_ulong id TSRMLS_DC);
JONJDBG_API jonjdbg_breakbase_t *jonjdbg_find_breakbase_ex(zend_ulong id, HashTable ***table, HashPosition *position TSRMLS_DC); /* }}} */

/* {{{ Breakpoint Exportation API */
JONJDBG_API void jonjdbg_export_breakpoints(FILE *handle TSRMLS_DC); /* }}} */

#endif /* JONJDBG_BP_H */
