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

#include "jonjdbg.h"
#include "jonjdbg_print.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_opcode.h"
#include "jonjdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

#define JONJDBG_PRINT_COMMAND_D(f, h, a, m, l, s) \
	JONJDBG_COMMAND_D_EXP(f, h, a, m, l, s, &jonjdbg_prompt_commands[9])

const jonjdbg_command_t jonjdbg_print_commands[] = {
	JONJDBG_PRINT_COMMAND_D(exec,       "print out the instructions in the execution context",  'e', print_exec,   NULL, 0),
	JONJDBG_PRINT_COMMAND_D(opline,     "print out the instruction in the current opline",      'o', print_opline, NULL, 0),
	JONJDBG_PRINT_COMMAND_D(class,      "print out the instructions in the specified class",    'c', print_class,  NULL, "s"),
	JONJDBG_PRINT_COMMAND_D(method,     "print out the instructions in the specified method",   'm', print_method, NULL, "m"),
	JONJDBG_PRINT_COMMAND_D(func,       "print out the instructions in the specified function", 'f', print_func,   NULL, "s"),
	JONJDBG_PRINT_COMMAND_D(stack,      "print out the instructions in the current stack",      's', print_stack,  NULL, 0),
	JONJDBG_END_COMMAND
};

JONJDBG_PRINT(opline) /* {{{ */
{
	if (EG(in_execution) && EG(current_execute_data)) {
		jonjdbg_print_opline(EG(current_execute_data), 1 TSRMLS_CC);
	} else {
		jonjdbg_error("Not Executing!");
	}

	return SUCCESS;
} /* }}} */

static inline void jonjdbg_print_function_helper(zend_function *method TSRMLS_DC) /* {{{ */
{
	switch (method->type) {
		case ZEND_USER_FUNCTION: {
			zend_op_array* op_array = &(method->op_array);
			HashTable vars;
			
			if (op_array) {
				zend_op *opline = &(op_array->opcodes[0]);
				zend_uint opcode = 0,
				end = op_array->last-1;

				if (method->common.scope) {
					jonjdbg_writeln("\tL%d-%d %s::%s() %s",
						op_array->line_start, op_array->line_end,
						method->common.scope->name,
						method->common.function_name,
						op_array->filename ? op_array->filename : "unknown");
				} else {
					jonjdbg_writeln("\tL%d-%d %s() %s",
						method->common.function_name ? op_array->line_start : 0, 
						method->common.function_name ? op_array->line_end : 0,
						method->common.function_name ? method->common.function_name : "{main}",
						op_array->filename ? op_array->filename : "unknown");
				}

				zend_hash_init(&vars, op_array->last, NULL, NULL, 0);
				do {
					char *decode = jonjdbg_decode_opline(op_array, opline, &vars TSRMLS_CC);
					if (decode != NULL) {
						jonjdbg_writeln("\t\tL%u\t%p %-30s %s", 
							opline->lineno,
							opline, 
							jonjdbg_decode_opcode(opline->opcode),
							decode);
						free(decode);
					} else {
						jonjdbg_error("\tFailed to decode opline %16p", opline);
					}
					opline++;
				} while (opcode++ < end);
				zend_hash_destroy(&vars);
			}
		} break;

		default: {
			if (method->common.scope) {
				jonjdbg_writeln("\tInternal %s::%s()", method->common.scope->name, method->common.function_name);
			} else {
				jonjdbg_writeln("\tInternal %s()", method->common.function_name);
			}
		}
	}
} /* }}} */

JONJDBG_PRINT(exec) /* {{{ */
{
	if (JONJDBG_G(exec)) {
		if (!JONJDBG_G(ops)) {
			jonjdbg_compile(TSRMLS_C);
		}

		if (JONJDBG_G(ops)) {
			jonjdbg_notice("Context %s", JONJDBG_G(exec));

			jonjdbg_print_function_helper((zend_function*) JONJDBG_G(ops) TSRMLS_CC);
		}
	} else {
		jonjdbg_error("No execution context set");
	}

return SUCCESS;
} /* }}} */

JONJDBG_PRINT(stack) /* {{{ */
{
	zend_op_array *ops = EG(active_op_array);
	
	if (EG(in_execution) && ops) {
		if (ops->function_name) {
			if (ops->scope) {
				jonjdbg_notice("Stack in %s::%s()", ops->scope->name, ops->function_name);
			} else {
				jonjdbg_notice("Stack in %s()", ops->function_name);
			}
		} else {
			if (ops->filename) {
				jonjdbg_notice("Stack in %s", ops->filename);
			} else {
				jonjdbg_notice("Stack @ %p", ops);
			}
		}
		jonjdbg_print_function_helper((zend_function*) ops TSRMLS_CC);
	} else {
		jonjdbg_error("Not Executing!");
	}

	return SUCCESS;
} /* }}} */

JONJDBG_PRINT(class) /* {{{ */
{
	zend_class_entry **ce;

	if (zend_lookup_class(param->str, param->len, &ce TSRMLS_CC) == SUCCESS) {
		jonjdbg_notice("%s %s: %s",
			((*ce)->type == ZEND_USER_CLASS) ?
				"User" : "Internal",
			((*ce)->ce_flags & ZEND_ACC_INTERFACE) ?
				"Interface" :
				((*ce)->ce_flags & ZEND_ACC_ABSTRACT) ?
					"Abstract Class" :
					"Class",
			(*ce)->name);

		jonjdbg_writeln("Methods (%d):", zend_hash_num_elements(&(*ce)->function_table));
		if (zend_hash_num_elements(&(*ce)->function_table)) {
			HashPosition position;
			zend_function *method;

			for (zend_hash_internal_pointer_reset_ex(&(*ce)->function_table, &position);
			     zend_hash_get_current_data_ex(&(*ce)->function_table, (void**) &method, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&(*ce)->function_table, &position)) {
				jonjdbg_print_function_helper(method TSRMLS_CC);
			}
		}
	} else {
		jonjdbg_error("The class %s could not be found", param->str);
	}

	return SUCCESS;
} /* }}} */

JONJDBG_PRINT(method) /* {{{ */
{
	zend_class_entry **ce;

	if (zend_lookup_class(param->method.class, strlen(param->method.class), &ce TSRMLS_CC) == SUCCESS) {
		zend_function *fbc;
		char *lcname = zend_str_tolower_dup(param->method.name, strlen(param->method.name));

		if (zend_hash_find(&(*ce)->function_table, lcname, strlen(lcname)+1, (void**)&fbc) == SUCCESS) {
			jonjdbg_notice("%s Method %s",
				(fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal",
				fbc->common.function_name);

			jonjdbg_print_function_helper(fbc TSRMLS_CC);
		} else {
			jonjdbg_error("The method %s could not be found", param->method.name);
		}

		efree(lcname);
	} else {
		jonjdbg_error("The class %s could not be found", param->method.class);
	}

	return SUCCESS;
} /* }}} */

JONJDBG_PRINT(func) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function* fbc;
	const char *func_name = param->str;
	size_t func_name_len = param->len;
	char *lcname;
	/* search active scope if begins with period */
	if (func_name[0] == '.') {
		if (EG(scope)) {
			func_name++;
			func_name_len--;

			func_table = &EG(scope)->function_table;
		} else {
			jonjdbg_error("No active class");
			return SUCCESS;
		}
	} else if (!EG(function_table)) {
		jonjdbg_error("No function table loaded");
		return SUCCESS;
	} else {
		func_table = EG(function_table);
	}

	lcname  = zend_str_tolower_dup(func_name, func_name_len);

	if (zend_hash_find(func_table, lcname, strlen(lcname)+1, (void**)&fbc) == SUCCESS) {
		jonjdbg_notice("%s %s %s",
			(fbc->type == ZEND_USER_FUNCTION) ? "User" : "Internal",
			(fbc->common.scope) ? "Method" : "Function",
			fbc->common.function_name);

		jonjdbg_print_function_helper(fbc TSRMLS_CC);
	} else {
		jonjdbg_error("The function %s could not be found", func_name);
	}

	efree(lcname);

	return SUCCESS;
} /* }}} */
