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

#include "zend.h"
#include "jonjdbg.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_frame.h"
#include "jonjdbg_list.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

void jonjdbg_restore_frame(TSRMLS_D) /* {{{ */
{
	if (JONJDBG_FRAME(num) == 0) {
		return;
	}

	JONJDBG_FRAME(num) = 0;

	/* move things back */
	EG(current_execute_data) = JONJDBG_FRAME(execute_data);

	EG(opline_ptr) = &JONJDBG_EX(opline);
	EG(active_op_array) = JONJDBG_EX(op_array);
	EG(return_value_ptr_ptr) = JONJDBG_EX(original_return_value);
	EG(active_symbol_table) = JONJDBG_EX(symbol_table);
	EG(This) = JONJDBG_EX(current_this);
	EG(scope) = JONJDBG_EX(current_scope);
	EG(called_scope) = JONJDBG_EX(current_called_scope);
} /* }}} */

void jonjdbg_switch_frame(int frame TSRMLS_DC) /* {{{ */
{
	zend_execute_data *execute_data = JONJDBG_FRAME(num)?JONJDBG_FRAME(execute_data):EG(current_execute_data);
	int i = 0;

	if (JONJDBG_FRAME(num) == frame) {
		jonjdbg_notice("Already in frame #%d", frame);
		return;
	}

	while (execute_data) {
		if (i++ == frame) {
			break;
		}

		do {
			execute_data = execute_data->prev_execute_data;
		} while (execute_data && execute_data->opline == NULL);
	}

	if (execute_data == NULL) {
		jonjdbg_error("No frame #%d", frame);
		return;
	}

	jonjdbg_restore_frame(TSRMLS_C);

	if (frame > 0) {
		JONJDBG_FRAME(num) = frame;

		/* backup things and jump back */
		JONJDBG_FRAME(execute_data) = EG(current_execute_data);
		EG(current_execute_data) = execute_data;

		EG(opline_ptr) = &JONJDBG_EX(opline);
		EG(active_op_array) = JONJDBG_EX(op_array);
		JONJDBG_FRAME(execute_data)->original_return_value = EG(return_value_ptr_ptr);
		EG(return_value_ptr_ptr) = JONJDBG_EX(original_return_value);
		EG(active_symbol_table) = JONJDBG_EX(symbol_table);
		EG(This) = JONJDBG_EX(current_this);
		EG(scope) = JONJDBG_EX(current_scope);
		EG(called_scope) = JONJDBG_EX(current_called_scope);
	}

	jonjdbg_notice("Switched to frame #%d", frame);
	jonjdbg_list_file(
		zend_get_executed_filename(TSRMLS_C),
		3,
		zend_get_executed_lineno(TSRMLS_C)-1,
		zend_get_executed_lineno(TSRMLS_C)
		TSRMLS_CC
	);
} /* }}} */

static void jonjdbg_dump_prototype(zval **tmp TSRMLS_DC) /* {{{ */
{
	zval **funcname, **class, **type, **args, **argstmp;
	char is_class;

	zend_hash_find(Z_ARRVAL_PP(tmp), "function", sizeof("function"),
		(void **)&funcname);

	if ((is_class = zend_hash_find(Z_ARRVAL_PP(tmp),
		"object", sizeof("object"), (void **)&class)) == FAILURE) {
		is_class = zend_hash_find(Z_ARRVAL_PP(tmp), "class", sizeof("class"),
			(void **)&class);
	} else {
		zend_get_object_classname(*class, (const char **)&Z_STRVAL_PP(class),
			(zend_uint *)&Z_STRLEN_PP(class) TSRMLS_CC);
	}

	if (is_class == SUCCESS) {
		zend_hash_find(Z_ARRVAL_PP(tmp), "type", sizeof("type"), (void **)&type);
	}

	jonjdbg_write("%s%s%s(",
		is_class == FAILURE?"":Z_STRVAL_PP(class),
		is_class == FAILURE?"":Z_STRVAL_PP(type),
		Z_STRVAL_PP(funcname)
	);

	if (zend_hash_find(Z_ARRVAL_PP(tmp), "args", sizeof("args"),
		(void **)&args) == SUCCESS) {
		HashPosition iterator;
		const zend_function *func = jonjdbg_get_function(
			Z_STRVAL_PP(funcname), is_class == FAILURE ? NULL : Z_STRVAL_PP(class) TSRMLS_CC);
		const zend_arg_info *arginfo = func ? func->common.arg_info : NULL;
		int j = 0, m = func ? func->common.num_args : 0;
		zend_bool is_variadic = 0;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(args), &iterator);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(args),
			(void **) &argstmp, &iterator) == SUCCESS) {
			if (j) {
				jonjdbg_write(", ");
			}
			if (m && j < m) {
#if JONJ_VERSION_ID >= 50600
				is_variadic = arginfo[j].is_variadic;
#endif
				jonjdbg_write("%s=%s",
					arginfo[j].name, is_variadic ? "[": "");
			}
			++j;

			zend_print_flat_zval_r(*argstmp TSRMLS_CC);
			zend_hash_move_forward_ex(Z_ARRVAL_PP(args), &iterator);
		}
		if (is_variadic) {
			jonjdbg_write("]");
		}
	}
	jonjdbg_write(")");
}

void jonjdbg_dump_backtrace(size_t num TSRMLS_DC) /* {{{ */
{
	zval zbacktrace;
	zval **tmp;
	zval **file, **line;
	HashPosition position;
	int i = 0, limit = num;
	int user_defined;

	if (limit < 0) {
		jonjdbg_error("Invalid backtrace size %d", limit);
	}

	zend_fetch_debug_backtrace(
		&zbacktrace, 0, 0, limit TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(zbacktrace), &position);
	zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace), (void**)&tmp, &position);
	while (1) {
		user_defined = zend_hash_find(Z_ARRVAL_PP(tmp), "file", sizeof("file"), (void **)&file);
		zend_hash_find(Z_ARRVAL_PP(tmp), "line", sizeof("line"), (void **)&line);
		zend_hash_move_forward_ex(Z_ARRVAL(zbacktrace), &position);

		if (zend_hash_get_current_data_ex(Z_ARRVAL(zbacktrace),
			(void**)&tmp, &position) == FAILURE) {
			jonjdbg_write("frame #%d: {main} at %s:%ld", i, Z_STRVAL_PP(file), Z_LVAL_PP(line));
			break;
		}

		if (user_defined == SUCCESS) {
			jonjdbg_write("frame #%d: ", i++);
			jonjdbg_dump_prototype(tmp TSRMLS_CC);
			jonjdbg_writeln(" at %s:%ld", Z_STRVAL_PP(file), Z_LVAL_PP(line));
		} else {
			jonjdbg_write(" => ");
			jonjdbg_dump_prototype(tmp TSRMLS_CC);
			jonjdbg_writeln(" (internal function)");
		}
	}

	jonjdbg_writeln(EMPTY);
	zval_dtor(&zbacktrace);
} /* }}} */
