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

#include "jonj.h"
#include "jonjdbg.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_info.h"
#include "jonjdbg_bp.h"
#include "jonjdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

#define JONJDBG_INFO_COMMAND_D(f, h, a, m, l, s) \
	JONJDBG_COMMAND_D_EXP(f, h, a, m, l, s, &jonjdbg_prompt_commands[14])

const jonjdbg_command_t jonjdbg_info_commands[] = {
	JONJDBG_INFO_COMMAND_D(break,    "show breakpoints",              'b', info_break,   NULL, 0),
	JONJDBG_INFO_COMMAND_D(files,    "show included files",           'F', info_files,   NULL, 0),
	JONJDBG_INFO_COMMAND_D(classes,  "show loaded classes",           'c', info_classes, NULL, 0),
	JONJDBG_INFO_COMMAND_D(funcs,    "show loaded classes",           'f', info_funcs,   NULL, 0),
	JONJDBG_INFO_COMMAND_D(error,    "show last error",               'e', info_error,   NULL, 0),
	JONJDBG_INFO_COMMAND_D(vars,     "show active variables",         'v', info_vars,    NULL, 0),
	JONJDBG_INFO_COMMAND_D(literal,  "show active literal constants", 'l', info_literal, NULL, 0),
	JONJDBG_INFO_COMMAND_D(memory,   "show memory manager stats",     'm', info_memory,  NULL, 0),
	JONJDBG_END_COMMAND
};

JONJDBG_INFO(break) /* {{{ */
{
	jonjdbg_print_breakpoints(JONJDBG_BREAK_FILE TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_SYM TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_METHOD TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_OPLINE TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_FILE_OPLINE TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_FUNCTION_OPLINE TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_METHOD_OPLINE TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_COND TSRMLS_CC);
	jonjdbg_print_breakpoints(JONJDBG_BREAK_OPCODE TSRMLS_CC);

	return SUCCESS;
} /* }}} */

JONJDBG_INFO(files) /* {{{ */
{
	HashPosition pos;
	char *fname;

	jonjdbg_notice("Included files: %d",
		zend_hash_num_elements(&EG(included_files)));

	zend_hash_internal_pointer_reset_ex(&EG(included_files), &pos);
	while (zend_hash_get_current_key_ex(&EG(included_files), &fname,
		NULL, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
		jonjdbg_writeln("File: %s", fname);
		zend_hash_move_forward_ex(&EG(included_files), &pos);
	}

	return SUCCESS;
} /* }}} */

JONJDBG_INFO(error) /* {{{ */
{
	if (PG(last_error_message)) {
		jonjdbg_writeln("Last error: %s at %s line %d",
			PG(last_error_message), PG(last_error_file), PG(last_error_lineno));
	} else {
		jonjdbg_notice("No error found!");
	}
	return SUCCESS;
} /* }}} */

JONJDBG_INFO(vars) /* {{{ */
{
	HashTable vars;
	HashPosition pos;
	char *var;
	zval **data;

	if (!EG(active_op_array)) {
		jonjdbg_error("No active op array!");
		return SUCCESS;
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);

		if (!EG(active_symbol_table)) {
			jonjdbg_error("No active symbol table!");
			return SUCCESS;
		}
	}

	zend_hash_init(&vars, 8, NULL, NULL, 0);

	zend_hash_internal_pointer_reset_ex(EG(active_symbol_table), &pos);
	while (zend_hash_get_current_key_ex(EG(active_symbol_table), &var,
		NULL, NULL, 0, &pos) == HASH_KEY_IS_STRING) {
		zend_hash_get_current_data_ex(EG(active_symbol_table), (void **)&data, &pos);
		if (*var != '_') {
			zend_hash_update(
				&vars, var, strlen(var)+1, (void**)data, sizeof(zval*), NULL);
		}
		zend_hash_move_forward_ex(EG(active_symbol_table), &pos);
	}

	{
		zend_op_array *ops = EG(active_op_array);
		
		if (ops->function_name) {
			if (ops->scope) {
				jonjdbg_notice(
				"Variables in %s::%s() (%d)", ops->scope->name, ops->function_name, zend_hash_num_elements(&vars));
			} else {
				jonjdbg_notice(
					"Variables in %s() (%d)", ops->function_name, zend_hash_num_elements(&vars));
			}
		} else {
			if (ops->filename) {
				jonjdbg_notice(
				"Variables in %s (%d)", ops->filename, zend_hash_num_elements(&vars));
			} else {
				jonjdbg_notice(
					"Variables @ %p (%d)", ops, zend_hash_num_elements(&vars));
			}
		}
	}

	if (zend_hash_num_elements(&vars)) {
		jonjdbg_writeln("Address\t\tRefs\tType\t\tVariable");
		for (zend_hash_internal_pointer_reset_ex(&vars, &pos);
			zend_hash_get_current_data_ex(&vars, (void**) &data, &pos) == SUCCESS;
			zend_hash_move_forward_ex(&vars, &pos)) {
			char *var;

			zend_hash_get_current_key_ex(&vars, &var, NULL, NULL, 0, &pos);

			if (*data) {
				jonjdbg_write(
				"%p\t%d\t",
					*data,
					Z_REFCOUNT_PP(data));

				switch (Z_TYPE_PP(data)) {
					case IS_STRING: 	jonjdbg_write("(string)\t"); 	break;
					case IS_LONG: 		jonjdbg_write("(integer)\t"); 	break;
					case IS_DOUBLE: 	jonjdbg_write("(float)\t"); 		break;
					case IS_RESOURCE:	jonjdbg_write("(resource)\t"); 	break;
					case IS_ARRAY:		jonjdbg_write("(array)\t"); 		break;
					case IS_OBJECT:		jonjdbg_write("(object)\t"); 	break;
					case IS_NULL:		jonjdbg_write("(null)\t"); 		break;
				}

				if (Z_TYPE_PP(data) == IS_RESOURCE) {
					int type;

					jonjdbg_writeln(
						"%s$%s", Z_ISREF_PP(data) ? "&": "", var);
					if (zend_list_find(Z_RESVAL_PP(data), &type)) {
						jonjdbg_write(
							"|-------(typeof)------> (%s)",
							zend_rsrc_list_get_rsrc_type(type TSRMLS_CC));
					} else {
						jonjdbg_write(
							"|-------(typeof)------> (unknown)");
					}
					jonjdbg_writeln(EMPTY);
				} else if (Z_TYPE_PP(data) == IS_OBJECT) {
					jonjdbg_writeln(
						"%s$%s", Z_ISREF_PP(data) ? "&": "", var);
					jonjdbg_write(
						"|-----(instanceof)----> (%s)", Z_OBJCE_PP(data)->name);
					jonjdbg_writeln(EMPTY);
				} else {
					jonjdbg_write(
						"%s$%s", Z_ISREF_PP(data) ? "&": "", var);
				}
			} else {
				jonjdbg_write(
					"n/a\tn/a\tn/a\t$%s", var);
			}
			jonjdbg_writeln(EMPTY);
		}
	}

	zend_hash_destroy(&vars);

	return SUCCESS;
} /* }}} */

JONJDBG_INFO(literal) /* {{{ */
{
	if ((EG(in_execution) && EG(active_op_array)) || JONJDBG_G(ops)) {
		zend_op_array *ops = EG(active_op_array) ? EG(active_op_array) : JONJDBG_G(ops);
		int literal = 0, count = ops->last_literal-1;

		if (ops->function_name) {
			if (ops->scope) {
				jonjdbg_notice(
				"Literal Constants in %s::%s() (%d)", ops->scope->name, ops->function_name, count);
			} else {
				jonjdbg_notice(
					"Literal Constants in %s() (%d)", ops->function_name, count);
			}
		} else {
			if (ops->filename) {
				jonjdbg_notice(
				"Literal Constants in %s (%d)", ops->filename, count);
			} else {
				jonjdbg_notice(
					"Literal Constants @ %p (%d)", ops, count);
			}
		}

		while (literal < ops->last_literal) {
			if (Z_TYPE(ops->literals[literal].constant) != IS_NULL) {
				jonjdbg_write("|-------- C%u -------> [", literal);
				zend_print_zval(
					&ops->literals[literal].constant, 0);
				jonjdbg_write("]");
				jonjdbg_writeln(EMPTY);
			}
			literal++;
		}
	} else {
		jonjdbg_error("Not executing!");
	}

	return SUCCESS;
} /* }}} */

JONJDBG_INFO(memory) /* {{{ */
{
	if (is_zend_mm(TSRMLS_C)) {
		jonjdbg_notice("Memory Manager Information");
		jonjdbg_notice("Current");
		jonjdbg_writeln("|-------> Used:\t%.3f kB", 
			(float) (zend_memory_usage(0 TSRMLS_CC)/1024));
		jonjdbg_writeln("|-------> Real:\t%.3f kB", 
			(float) (zend_memory_usage(1 TSRMLS_CC)/1024));
		jonjdbg_notice("Peak");
		jonjdbg_writeln("|-------> Used:\t%.3f kB", 
			(float) (zend_memory_peak_usage(0 TSRMLS_CC)/1024));
		jonjdbg_writeln("|-------> Real:\t%.3f kB", 
			(float) (zend_memory_peak_usage(1 TSRMLS_CC)/1024));
	} else {
		jonjdbg_error("Memory Manager Disabled!");
	}
	return SUCCESS;
} /* }}} */

static inline void jonjdbg_print_class_name(zend_class_entry **ce TSRMLS_DC) /* {{{ */
{
	jonjdbg_write(
		"%s %s %s (%d)",
		((*ce)->type == ZEND_USER_CLASS) ?
			"User" : "Internal",
		((*ce)->ce_flags & ZEND_ACC_INTERFACE) ?
			"Interface" :
			((*ce)->ce_flags & ZEND_ACC_ABSTRACT) ?
				"Abstract Class" :
					"Class",
		(*ce)->name, zend_hash_num_elements(&(*ce)->function_table));
} /* }}} */

JONJDBG_INFO(classes) /* {{{ */
{
	HashPosition position;
	zend_class_entry **ce;
	HashTable classes;

	zend_hash_init(&classes, 8, NULL, NULL, 0);

	for (zend_hash_internal_pointer_reset_ex(EG(class_table), &position);
		zend_hash_get_current_data_ex(EG(class_table), (void**)&ce, &position) == SUCCESS;
		zend_hash_move_forward_ex(EG(class_table), &position)) {

		if ((*ce)->type == ZEND_USER_CLASS) {
			zend_hash_next_index_insert(
				&classes, ce, sizeof(ce), NULL);
		}
	}

	jonjdbg_notice("User Classes (%d)",
		zend_hash_num_elements(&classes));

	for (zend_hash_internal_pointer_reset_ex(&classes, &position);
		zend_hash_get_current_data_ex(&classes, (void**)&ce, &position) == SUCCESS;
		zend_hash_move_forward_ex(&classes, &position)) {

		jonjdbg_print_class_name(ce TSRMLS_CC);
		jonjdbg_writeln(EMPTY);

		if ((*ce)->parent) {
			zend_class_entry *pce = (*ce)->parent;
			do {
				jonjdbg_write("|-------- ");
				jonjdbg_print_class_name(&pce TSRMLS_CC);
				jonjdbg_writeln(EMPTY);
			} while ((pce = pce->parent));
		}

		if ((*ce)->info.user.filename) {
			jonjdbg_writeln(
				"|---- in %s on line %u",
				(*ce)->info.user.filename,
				(*ce)->info.user.line_start);
		} else {
			jonjdbg_writeln("|---- no source code");
		}
	    	jonjdbg_writeln(EMPTY);
	}

	zend_hash_destroy(&classes);

	return SUCCESS;
} /* }}} */

JONJDBG_INFO(funcs) /* {{{ */
{
	HashPosition position;
	zend_function *zf, **pzf;
	HashTable functions;

	zend_hash_init(&functions, 8, NULL, NULL, 0);

	for (zend_hash_internal_pointer_reset_ex(EG(function_table), &position);
		zend_hash_get_current_data_ex(EG(function_table), (void**)&zf, &position) == SUCCESS;
		zend_hash_move_forward_ex(EG(function_table), &position)) {

		if (zf->type == ZEND_USER_FUNCTION) {
			zend_hash_next_index_insert(
				&functions, (void**) &zf, sizeof(zend_function), NULL);
		}
	}

	jonjdbg_notice("User Functions (%d)",
		zend_hash_num_elements(&functions));

	for (zend_hash_internal_pointer_reset_ex(&functions, &position);
		zend_hash_get_current_data_ex(&functions, (void**)&pzf, &position) == SUCCESS;
		zend_hash_move_forward_ex(&functions, &position)) {
		zend_op_array *op_array = &((*pzf)->op_array);

		jonjdbg_writeln(
			"|-------- %s in %s on line %d",
			op_array->function_name ? op_array->function_name : "{main}",
			op_array->filename ? op_array->filename : "(no source code)",
			op_array->line_start);
	}

	zend_hash_destroy(&functions);

	return SUCCESS;
} /* }}} */
