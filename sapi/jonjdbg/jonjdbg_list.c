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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifndef _WIN32
#	include <sys/mman.h>
#	include <unistd.h>
#endif
#include <fcntl.h>
#include "jonjdbg.h"
#include "jonjdbg_list.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_prompt.h"
#include "jonj_streams.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

#define JONJDBG_LIST_COMMAND_D(f, h, a, m, l, s) \
	JONJDBG_COMMAND_D_EXP(f, h, a, m, l, s, &jonjdbg_prompt_commands[13])

const jonjdbg_command_t jonjdbg_list_commands[] = {
	JONJDBG_LIST_COMMAND_D(lines,     "lists the specified lines",    'l', list_lines,  NULL, "l"),
	JONJDBG_LIST_COMMAND_D(class,     "lists the specified class",    'c', list_class,  NULL, "s"),
	JONJDBG_LIST_COMMAND_D(method,    "lists the specified method",   'm', list_method, NULL, "m"),
	JONJDBG_LIST_COMMAND_D(func,      "lists the specified function", 'f', list_func,   NULL, "s"),
	JONJDBG_END_COMMAND
};

JONJDBG_LIST(lines) /* {{{ */
{
	if (!JONJDBG_G(exec) && !zend_is_executing(TSRMLS_C)) {
		jonjdbg_error("Not executing, and execution context not set");
		return SUCCESS;
	}

	switch (param->type) {
		case NUMERIC_PARAM:
			jonjdbg_list_file(jonjdbg_current_file(TSRMLS_C),
				(param->num < 0 ? 1 - param->num : param->num),
				(param->num < 0 ? param->num : 0) + zend_get_executed_lineno(TSRMLS_C),
				0 TSRMLS_CC);
			break;
			
		case FILE_PARAM:
			jonjdbg_list_file(param->file.name, param->file.line, 0, 0 TSRMLS_CC);
			break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

JONJDBG_LIST(func) /* {{{ */
{
	jonjdbg_list_function_byname(param->str, param->len TSRMLS_CC);

	return SUCCESS;
} /* }}} */

JONJDBG_LIST(method) /* {{{ */
{
	zend_class_entry **ce;

	if (zend_lookup_class(param->method.class, strlen(param->method.class), &ce TSRMLS_CC) == SUCCESS) {
		zend_function *function;
		char *lcname = zend_str_tolower_dup(param->method.name, strlen(param->method.name));

		if (zend_hash_find(&(*ce)->function_table, lcname, strlen(lcname)+1, (void**) &function) == SUCCESS) {
			jonjdbg_list_function(function TSRMLS_CC);
		} else {
			jonjdbg_error("Could not find %s::%s", param->method.class, param->method.name);
		}

		efree(lcname);
	} else {
		jonjdbg_error("Could not find the class %s", param->method.class);
	}

	return SUCCESS;
} /* }}} */

JONJDBG_LIST(class) /* {{{ */
{
	zend_class_entry **ce;

	if (zend_lookup_class(param->str, param->len, &ce TSRMLS_CC) == SUCCESS) {
		if ((*ce)->type == ZEND_USER_CLASS) {
			if ((*ce)->info.user.filename) {
				jonjdbg_list_file(
					(*ce)->info.user.filename,
					(*ce)->info.user.line_end - (*ce)->info.user.line_start + 1,
					(*ce)->info.user.line_start, 0 TSRMLS_CC
				);
			} else {
				jonjdbg_error("The source of the requested class (%s) cannot be found", (*ce)->name);
			}
		} else {
			jonjdbg_error("The class requested (%s) is not user defined", (*ce)->name);
		}
	} else {
		jonjdbg_error("The requested class (%s) could not be found", param->str);
	}

	return SUCCESS;
} /* }}} */

void jonjdbg_list_file(const char *filename, long count, long offset, int highlight TSRMLS_DC) /* {{{ */
{
	struct stat st;
	char *opened = NULL;
	char buffer[8096] = {0,};
	long line = 0;

	jonj_stream *stream = NULL;
	
	if (VCWD_STAT(filename, &st) == FAILURE) {
		jonjdbg_error("Failed to stat file %s", filename);
		return;
	}

	stream = jonj_stream_open_wrapper(filename, "rb", USE_PATH, &opened);
	
	if (!stream) {
		jonjdbg_error("Failed to open file %s to list", filename);
		return;
	}
	
	if (offset < 0) {
		count += offset;
		offset = 0;
	}
	
	while (jonj_stream_gets(stream, buffer, sizeof(buffer)) != NULL) {
		long linelen = strlen(buffer);

		++line;
		
		if (offset <= line) {
			if (!highlight) {
				jonjdbg_write("%05ld: %s", line, buffer);
			} else {
				if (highlight != line) {
					jonjdbg_write(" %05ld: %s", line, buffer);
				} else {
					jonjdbg_write(">%05ld: %s", line, buffer);
				}
			}

			if (buffer[linelen - 1] != '\n') {
				jonjdbg_write("\n");
			}
		}

		if (count > 0 && count + offset - 1 < line) {
			break;
		}
	}
	
	jonj_stream_close(stream);
} /* }}} */

void jonjdbg_list_function(const zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const zend_op_array *ops;

	if (fbc->type != ZEND_USER_FUNCTION) {
		jonjdbg_error("The function requested (%s) is not user defined", fbc->common.function_name);
		return;
	}

	ops = (zend_op_array*)fbc;

	jonjdbg_list_file(ops->filename,
		ops->line_end - ops->line_start + 1, ops->line_start, 0 TSRMLS_CC);
} /* }}} */

void jonjdbg_list_function_byname(const char *str, size_t len TSRMLS_DC) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function* fbc;
	char *func_name = (char*) str;
	size_t func_name_len = len;

	/* search active scope if begins with period */
	if (func_name[0] == '.') {
		if (EG(scope)) {
			func_name++;
			func_name_len--;

			func_table = &EG(scope)->function_table;
		} else {
			jonjdbg_error("No active class");
			return;
		}
	} else if (!EG(function_table)) {
		jonjdbg_error("No function table loaded");
		return;
	} else {
		func_table = EG(function_table);
	}

	/* use lowercase names, case insensitive */
	func_name = zend_str_tolower_dup(func_name, func_name_len);

	if (zend_hash_find(func_table, func_name, func_name_len+1, (void**)&fbc) == SUCCESS) {
		jonjdbg_list_function(fbc TSRMLS_CC);
	} else {
		jonjdbg_error("Function %s not found", func_name);
	}

	efree(func_name);
} /* }}} */

