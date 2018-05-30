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
#include "zend_hash.h"
#include "jonjdbg.h"
#include "jonjdbg_bp.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_opcode.h"
#include "zend_globals.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

/* {{{ private api functions */
static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_file(zend_op_array* TSRMLS_DC);
static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_symbol(zend_function* TSRMLS_DC);
static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_method(zend_op_array* TSRMLS_DC);
static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_opline(jonjdbg_opline_ptr_t TSRMLS_DC);
static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_opcode(zend_uchar TSRMLS_DC);
static inline jonjdbg_breakbase_t *jonjdbg_find_conditional_breakpoint(zend_execute_data *execute_data TSRMLS_DC); /* }}} */

/*
* Note:
*	A break point must always set the correct id and type
*	A set breakpoint function must always map new points
*/
static inline void _jonjdbg_break_mapping(int id, HashTable *table TSRMLS_DC)
{
	zend_hash_index_update(
		&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], (id), (void**) &table, sizeof(void*), NULL);
}

#define JONJDBG_BREAK_MAPPING(id, table) _jonjdbg_break_mapping(id, table TSRMLS_CC)
#define JONJDBG_BREAK_UNMAPPING(id) \
	zend_hash_index_del(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], (id))

#define JONJDBG_BREAK_INIT(b, t) do {\
	b.id = JONJDBG_G(bp_count)++; \
	b.type = t; \
	b.disabled = 0;\
	b.hits = 0; \
} while(0)

static void jonjdbg_file_breaks_dtor(void *data) /* {{{ */
{
	jonjdbg_breakfile_t *bp = (jonjdbg_breakfile_t*) data;

	efree((char*)bp->filename);
} /* }}} */

static void jonjdbg_class_breaks_dtor(void *data) /* {{{ */
{
	jonjdbg_breakmethod_t *bp = (jonjdbg_breakmethod_t*) data;

	efree((char*)bp->class_name);
	efree((char*)bp->func_name);
} /* }}} */

static void jonjdbg_opline_class_breaks_dtor(void *data) /* {{{ */
{
	zend_hash_destroy((HashTable *)data);
} /* }}} */

static void jonjdbg_opline_breaks_dtor(void *data) /* {{{ */
{
	jonjdbg_breakopline_t *bp = (jonjdbg_breakopline_t *) data;

	if (bp->class_name) {
		efree((char*)bp->class_name);
	}
	if (bp->func_name) {
		efree((char*)bp->func_name);
	}
} /* }}} */

JONJDBG_API void jonjdbg_reset_breakpoints(TSRMLS_D) /* {{{ */
{
	if (zend_hash_num_elements(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP])) {
		HashPosition position[2];
		HashTable **table = NULL;

		for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], &position[0]);
			zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], (void**)&table, &position[0]) == SUCCESS;
			zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], &position[0])) {
			jonjdbg_breakbase_t *brake;

			for (zend_hash_internal_pointer_reset_ex((*table), &position[1]);
				zend_hash_get_current_data_ex((*table), (void**)&brake, &position[1]) == SUCCESS;
				zend_hash_move_forward_ex((*table), &position[1])) {
				brake->hits = 0;
			}
		}
	}
} /* }}} */

JONJDBG_API void jonjdbg_export_breakpoints(FILE *handle TSRMLS_DC) /* {{{ */
{
	HashPosition position[2];
	HashTable **table = NULL;
	zend_ulong id = 0L;

	if (zend_hash_num_elements(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP])) {
		jonjdbg_notice(
			"Exporting %d breakpoints",
			zend_hash_num_elements(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP]));
		/* this only looks like magic, it isn't */
		for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], &position[0]);
			zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], (void**)&table, &position[0]) == SUCCESS;
			zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], &position[0])) {
			jonjdbg_breakbase_t *brake;

			zend_hash_get_current_key_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], NULL, NULL, &id, 0, &position[0]);

			for (zend_hash_internal_pointer_reset_ex((*table), &position[1]);
				zend_hash_get_current_data_ex((*table), (void**)&brake, &position[1]) == SUCCESS;
				zend_hash_move_forward_ex((*table), &position[1])) {
				if (brake->id == id) {
					switch (brake->type) {
						case JONJDBG_BREAK_FILE: {
							fprintf(handle,
								"break %s:%lu\n",
								((jonjdbg_breakfile_t*)brake)->filename,
								((jonjdbg_breakfile_t*)brake)->line);
						} break;

						case JONJDBG_BREAK_SYM: {
							fprintf(handle,
								"break %s\n",
								((jonjdbg_breaksymbol_t*)brake)->symbol);
						} break;

						case JONJDBG_BREAK_METHOD: {
							fprintf(handle,
								"break %s::%s\n",
								((jonjdbg_breakmethod_t*)brake)->class_name,
								((jonjdbg_breakmethod_t*)brake)->func_name);
						} break;

						case JONJDBG_BREAK_METHOD_OPLINE: {
							fprintf(handle,
								"break %s::%s#%ld\n",
								((jonjdbg_breakopline_t*)brake)->class_name,
								((jonjdbg_breakopline_t*)brake)->func_name,
								((jonjdbg_breakopline_t*)brake)->opline_num);
						} break;

						case JONJDBG_BREAK_FUNCTION_OPLINE: {
							fprintf(handle,
								"break %s#%ld\n",
								((jonjdbg_breakopline_t*)brake)->func_name,
								((jonjdbg_breakopline_t*)brake)->opline_num);
						} break;

						case JONJDBG_BREAK_FILE_OPLINE: {
							fprintf(handle,
								"break %s:#%ld\n",
								((jonjdbg_breakopline_t*)brake)->class_name,
								((jonjdbg_breakopline_t*)brake)->opline_num);
						} break;

						case JONJDBG_BREAK_OPCODE: {
							fprintf(handle,
								"break %s\n",
								((jonjdbg_breakop_t*)brake)->name);
						} break;

						case JONJDBG_BREAK_COND: {
							jonjdbg_breakcond_t *conditional = (jonjdbg_breakcond_t*) brake;

							if (conditional->paramed) {
								switch (conditional->param.type) {
									case STR_PARAM:
										fprintf(handle,
											"break at %s if %s\n", conditional->param.str, conditional->code);
									break;

									case METHOD_PARAM:
										fprintf(handle,
											"break at %s::%s if %s\n",
											conditional->param.method.class, conditional->param.method.name,
											conditional->code);
									break;

									case FILE_PARAM:
										fprintf(handle,
											"break at %s:%lu if %s\n",
											conditional->param.file.name, conditional->param.file.line,
											conditional->code);
									break;

									default: { /* do nothing */ } break;
								}
							} else {
								fprintf(
									handle, "break if %s\n", conditional->code);
							}
						} break;
					}
				}
			}
		}
	}
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_file(const char *path, long line_num TSRMLS_DC) /* {{{ */
{
	jonj_stream_statbuf ssb;
	char realpath[MAXPATHLEN];
	
	if (jonj_stream_stat_path(path, &ssb) != FAILURE) {
		if (ssb.sb.st_mode & (S_IFREG|S_IFLNK)) {
			HashTable *broken;
			jonjdbg_breakfile_t new_break;
			size_t path_len = 0L;
			
			if (VCWD_REALPATH(path, realpath)) {
				path = realpath;
			}
			path_len = strlen(path);
			
			if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE],
				path, path_len, (void**)&broken) == FAILURE) {
				HashTable breaks;

				zend_hash_init(&breaks, 8, NULL, jonjdbg_file_breaks_dtor, 0);

				zend_hash_update(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE],
					path, path_len, &breaks, sizeof(HashTable),
					(void**)&broken);
			}

			if (!zend_hash_index_exists(broken, line_num)) {
				JONJDBG_G(flags) |= JONJDBG_HAS_FILE_BP;

				JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_FILE);
				new_break.filename = estrndup(path, path_len);
				new_break.line = line_num;

				zend_hash_index_update(
					broken, line_num, (void**)&new_break, sizeof(jonjdbg_breakfile_t), NULL);

				jonjdbg_notice("Breakpoint #%d added at %s:%ld",
					new_break.id, new_break.filename, new_break.line);

				JONJDBG_BREAK_MAPPING(new_break.id, broken);
			} else {
				jonjdbg_error("Breakpoint at %s:%ld exists", path, line_num);
			}

		} else {
			jonjdbg_error("Cannot set breakpoint in %s, it is not a regular file", path);
		}
	} else {
		jonjdbg_error("Cannot stat %s, it does not exist", path);
	}
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_symbol(const char *name, size_t name_len TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_exists(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], name, name_len)) {
		jonjdbg_breaksymbol_t new_break;

		JONJDBG_G(flags) |= JONJDBG_HAS_SYM_BP;

		JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_SYM);
		new_break.symbol = estrndup(name, name_len);

		zend_hash_update(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], new_break.symbol,
			name_len, &new_break, sizeof(jonjdbg_breaksymbol_t), NULL);

		jonjdbg_notice("Breakpoint #%d added at %s",
			new_break.id, new_break.symbol);

		JONJDBG_BREAK_MAPPING(new_break.id, &JONJDBG_G(bp)[JONJDBG_BREAK_SYM]);
	} else {
		jonjdbg_notice("Breakpoint exists at %s", name);
	}
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_method(const char *class_name, const char *func_name TSRMLS_DC) /* {{{ */
{
	HashTable class_breaks, *class_table;
	size_t class_len = strlen(class_name);
	size_t func_len = strlen(func_name);
	char *lcname = zend_str_tolower_dup(func_name, func_len);

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD], class_name,
		class_len, (void**)&class_table) != SUCCESS) {
		zend_hash_init(&class_breaks, 8, NULL, jonjdbg_class_breaks_dtor, 0);
		zend_hash_update(
			&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD],
			class_name, class_len,
			(void**)&class_breaks, sizeof(HashTable), (void**)&class_table);
	}

	if (!zend_hash_exists(class_table, lcname, func_len)) {
		jonjdbg_breakmethod_t new_break;

		JONJDBG_G(flags) |= JONJDBG_HAS_METHOD_BP;

		JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_METHOD);
		new_break.class_name = estrndup(class_name, class_len);
		new_break.class_len = class_len;
		new_break.func_name = estrndup(func_name, func_len);
		new_break.func_len = func_len;

		zend_hash_update(class_table, lcname, func_len,
			&new_break, sizeof(jonjdbg_breakmethod_t), NULL);

		jonjdbg_notice("Breakpoint #%d added at %s::%s",
			new_break.id, class_name, func_name);

		JONJDBG_BREAK_MAPPING(new_break.id, class_table);
	} else {
		jonjdbg_notice("Breakpoint exists at %s::%s", class_name, func_name);
	}

	efree(lcname);
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_opline(zend_ulong opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], opline)) {
		jonjdbg_breakline_t new_break;

		JONJDBG_G(flags) |= JONJDBG_HAS_OPLINE_BP;

		JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_OPLINE);
		new_break.name = NULL;
		new_break.opline = opline;
		new_break.base = NULL;

		zend_hash_index_update(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], opline,
			&new_break, sizeof(jonjdbg_breakline_t), NULL);

		jonjdbg_notice("Breakpoint #%d added at %#lx",
			new_break.id, new_break.opline);
		JONJDBG_BREAK_MAPPING(new_break.id, &JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]);
	} else {
		jonjdbg_notice("Breakpoint exists at %#lx", opline);
	}
} /* }}} */

JONJDBG_API int jonjdbg_resolve_op_array_break(jonjdbg_breakopline_t *brake, zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakline_t opline_break;
	if (op_array->last <= brake->opline_num) {
		if (brake->class_name == NULL) {
			jonjdbg_error("There are only %d oplines in function %s (breaking at opline %ld impossible)", op_array->last, brake->func_name, brake->opline_num);
		} else if (brake->func_name == NULL) {
			jonjdbg_error("There are only %d oplines in file %s (breaking at opline %ld impossible)", op_array->last, brake->class_name, brake->opline_num);
		} else {
			jonjdbg_error("There are only %d oplines in method %s::%s (breaking at opline %ld impossible)", op_array->last, brake->class_name, brake->func_name, brake->opline_num);
		}

		return FAILURE;
	}

	opline_break.disabled = 0;
	opline_break.hits = 0;
	opline_break.id = brake->id;
	opline_break.opline = brake->opline = (zend_ulong)(op_array->opcodes + brake->opline_num);
	opline_break.name = NULL;
	opline_break.base = brake;
	if (op_array->scope) {
		opline_break.type = JONJDBG_BREAK_METHOD_OPLINE;
	} else if (op_array->function_name) {
		opline_break.type = JONJDBG_BREAK_FUNCTION_OPLINE;
	} else {
		opline_break.type = JONJDBG_BREAK_FILE_OPLINE;
	}

	JONJDBG_G(flags) |= JONJDBG_HAS_OPLINE_BP;

	zend_hash_index_update(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], opline_break.opline, &opline_break, sizeof(jonjdbg_breakline_t), NULL);

	return SUCCESS;
} /* }}} */

JONJDBG_API void jonjdbg_resolve_op_array_breaks(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	HashTable *func_table = &JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE];
	HashTable *oplines_table;
	HashPosition position;
	jonjdbg_breakopline_t *brake;

	if (op_array->scope != NULL &&
	    zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE], op_array->scope->name, op_array->scope->name_length, (void **)&func_table) == FAILURE) {
		return;
	}

	if (op_array->function_name == NULL) {
		if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE], op_array->filename, strlen(op_array->filename), (void **)&oplines_table) == FAILURE) {
			return;
		}
	} else if (zend_hash_find(func_table, op_array->function_name?op_array->function_name:"", op_array->function_name?strlen(op_array->function_name):0, (void **)&oplines_table) == FAILURE) {
		return;
	}

	for (zend_hash_internal_pointer_reset_ex(oplines_table, &position);
	     zend_hash_get_current_data_ex(oplines_table, (void**) &brake, &position) == SUCCESS;
	     zend_hash_move_forward_ex(oplines_table, &position)) {
		if (jonjdbg_resolve_op_array_break(brake, op_array TSRMLS_CC) == SUCCESS) {
			jonjdbg_breakline_t *opline_break;

			zend_hash_internal_pointer_end(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]);
			zend_hash_get_current_data(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], (void **)&opline_break);

			jonjdbg_notice("Breakpoint #%d resolved at %s%s%s#%ld (opline %#lx)",
				brake->id,
				brake->class_name?brake->class_name:"",
				brake->class_name&&brake->func_name?"::":"",
				brake->func_name?brake->func_name:"",
				brake->opline_num,
				brake->opline);
		}
	}
} /* }}} */

JONJDBG_API int jonjdbg_resolve_opline_break(jonjdbg_breakopline_t *new_break TSRMLS_DC) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function *func;

	if (new_break->func_name == NULL) {
		if (EG(current_execute_data) == NULL) {
			if (JONJDBG_G(ops) != NULL && !memcmp(JONJDBG_G(ops)->filename, new_break->class_name, new_break->class_len)) {
				if (jonjdbg_resolve_op_array_break(new_break, JONJDBG_G(ops) TSRMLS_CC) == SUCCESS) {
					return SUCCESS;
				} else {
					return 2;
				}
			}
			return FAILURE;
		} else {
			zend_execute_data *execute_data = EG(current_execute_data);
			do {
				if (execute_data->op_array->function_name == NULL && execute_data->op_array->scope == NULL && !memcmp(execute_data->op_array->filename, new_break->class_name, new_break->class_len)) {
					if (jonjdbg_resolve_op_array_break(new_break, execute_data->op_array TSRMLS_CC) == SUCCESS) {
						return SUCCESS;
					} else {
						return 2;
					}
				}
			} while ((execute_data = execute_data->prev_execute_data) != NULL);
			return FAILURE;
		}
	}

	if (new_break->class_name != NULL) {
		zend_class_entry **ce;
		if (zend_hash_find(EG(class_table), zend_str_tolower_dup(new_break->class_name, new_break->class_len), new_break->class_len + 1, (void **)&ce) == FAILURE) {
			return FAILURE;
		}
		func_table = &(*ce)->function_table;
	}

	if (zend_hash_find(func_table, zend_str_tolower_dup(new_break->func_name, new_break->func_len), new_break->func_len + 1, (void **)&func) == FAILURE) {
		if (new_break->class_name != NULL && new_break->func_name != NULL) {
			jonjdbg_error("Method %s doesn't exist in class %s", new_break->func_name, new_break->class_name);
			return 2;
		}
		return FAILURE;
	}

	if (func->type != ZEND_USER_FUNCTION) {
		if (new_break->class_name == NULL) {
			jonjdbg_error("%s is not an user defined function, no oplines exist", new_break->func_name);
		} else {
			jonjdbg_error("%s::%s is not an user defined method, no oplines exist", new_break->class_name, new_break->func_name);
		}
		return 2;
	}

	if (jonjdbg_resolve_op_array_break(new_break, &func->op_array TSRMLS_CC) == FAILURE) {
		return 2;
	}

	return SUCCESS;
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_method_opline(const char *class, const char *method, zend_ulong opline TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakopline_t new_break;
	HashTable class_breaks, *class_table;
	HashTable method_breaks, *method_table;

	JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_METHOD_OPLINE);
	new_break.func_len = strlen(method);
	new_break.func_name = estrndup(method, new_break.func_len);
	new_break.class_len = strlen(class);
	new_break.class_name = estrndup(class, new_break.class_len);
	new_break.opline_num = opline;
	new_break.opline = 0;

	switch (jonjdbg_resolve_opline_break(&new_break TSRMLS_CC)) {
		case FAILURE:
			jonjdbg_notice("Pending breakpoint #%d at %s::%s#%ld", new_break.id, new_break.class_name, new_break.func_name, opline);
			break;

		case SUCCESS:
			jonjdbg_notice("Breakpoint #%d added at %s::%s#%ld", new_break.id, new_break.class_name, new_break.func_name, opline);
			break;

		case 2:
			return;
	}

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE], new_break.class_name, new_break.class_len, (void **)&class_table) == FAILURE) {
		zend_hash_init(&class_breaks, 8, NULL, jonjdbg_opline_class_breaks_dtor, 0);
		zend_hash_update(
			&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE],
			new_break.class_name,
			new_break.class_len,
			(void **)&class_breaks, sizeof(HashTable), (void **)&class_table);
	}

	if (zend_hash_find(class_table, new_break.func_name, new_break.func_len, (void **)&method_table) == FAILURE) {
		zend_hash_init(&method_breaks, 8, NULL, jonjdbg_opline_breaks_dtor, 0);
		zend_hash_update(
			class_table,
			new_break.func_name,
			new_break.func_len,
			(void **)&method_breaks, sizeof(HashTable), (void **)&method_table);
	}

	if (zend_hash_index_exists(method_table, opline)) {
		jonjdbg_notice("Breakpoint already exists for %s::%s#%ld", new_break.class_name, new_break.func_name, opline);
		efree((char*)new_break.func_name);
		efree((char*)new_break.class_name);
		JONJDBG_G(bp_count)--;
		return;
	}

	JONJDBG_G(flags) |= JONJDBG_HAS_METHOD_OPLINE_BP;

	JONJDBG_BREAK_MAPPING(new_break.id, method_table);

	zend_hash_index_update(method_table, opline, &new_break, sizeof(jonjdbg_breakopline_t), NULL);
}

JONJDBG_API void jonjdbg_set_breakpoint_function_opline(const char *function, zend_ulong opline TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakopline_t new_break;
	HashTable func_breaks, *func_table;

	JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_FUNCTION_OPLINE);
	new_break.func_len = strlen(function);
	new_break.func_name = estrndup(function, new_break.func_len);
	new_break.class_len = 0;
	new_break.class_name = NULL;
	new_break.opline_num = opline;
	new_break.opline = 0;

	switch (jonjdbg_resolve_opline_break(&new_break TSRMLS_CC)) {
		case FAILURE:
			jonjdbg_notice("Pending breakpoint #%d at %s#%ld", new_break.id, new_break.func_name, opline);
			break;

		case SUCCESS:
			jonjdbg_notice("Breakpoint #%d added at %s#%ld", new_break.id, new_break.func_name, opline);
			break;

		case 2:
			return;
	}

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE], new_break.func_name, new_break.func_len, (void **)&func_table) == FAILURE) {
		zend_hash_init(&func_breaks, 8, NULL, jonjdbg_opline_breaks_dtor, 0);
		zend_hash_update(
			&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE],
			new_break.func_name,
			new_break.func_len,
			(void **)&func_breaks, sizeof(HashTable), (void **)&func_table);
	}

	if (zend_hash_index_exists(func_table, opline)) {
		jonjdbg_notice("Breakpoint already exists for %s#%ld", new_break.func_name, opline);
		efree((char*)new_break.func_name);
		JONJDBG_G(bp_count)--;
		return;
	}

	JONJDBG_BREAK_MAPPING(new_break.id, func_table);

	JONJDBG_G(flags) |= JONJDBG_HAS_FUNCTION_OPLINE_BP;

	zend_hash_index_update(func_table, opline, &new_break, sizeof(jonjdbg_breakopline_t), NULL);
}

JONJDBG_API void jonjdbg_set_breakpoint_file_opline(const char *file, zend_ulong opline TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakopline_t new_break;
	HashTable file_breaks, *file_table;

	JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_FILE_OPLINE);
	new_break.func_len = 0;
	new_break.func_name = NULL;
	new_break.class_len = strlen(file);
	new_break.class_name = estrndup(file, new_break.class_len);
	new_break.opline_num = opline;
	new_break.opline = 0;

	switch (jonjdbg_resolve_opline_break(&new_break TSRMLS_CC)) {
		case FAILURE:
			jonjdbg_notice("Pending breakpoint #%d at %s:%ld", new_break.id, new_break.class_name, opline);
			break;

		case SUCCESS:
			jonjdbg_notice("Breakpoint #%d added at %s:%ld", new_break.id, new_break.class_name, opline);
			break;

		case 2:
			return;
	}

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE], new_break.class_name, new_break.class_len, (void **)&file_table) == FAILURE) {
		zend_hash_init(&file_breaks, 8, NULL, jonjdbg_opline_breaks_dtor, 0);
		zend_hash_update(
			&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE],
			new_break.class_name,
			new_break.class_len,
			(void **)&file_breaks, sizeof(HashTable), (void **)&file_table);
	}

	if (zend_hash_index_exists(file_table, opline)) {
		jonjdbg_notice("Breakpoint already exists for %s:%ld", new_break.class_name, opline);
		efree((char*)new_break.class_name);
		JONJDBG_G(bp_count)--;
		return;
	}

	JONJDBG_BREAK_MAPPING(new_break.id, file_table);

	JONJDBG_G(flags) |= JONJDBG_HAS_FILE_OPLINE_BP;

	zend_hash_index_update(file_table, opline, &new_break, sizeof(jonjdbg_breakopline_t), NULL);
}

JONJDBG_API void jonjdbg_set_breakpoint_opcode(const char *name, size_t name_len TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakop_t new_break;
	zend_ulong hash = zend_hash_func(name, name_len);

	if (zend_hash_index_exists(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE], hash)) {
		jonjdbg_notice(
			"Breakpoint exists for %s", name);
		return;
	}

	JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_OPCODE);
	new_break.hash = hash;
	new_break.name = estrndup(name, name_len);

	zend_hash_index_update(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE], hash,
		&new_break, sizeof(jonjdbg_breakop_t), NULL);

	JONJDBG_G(flags) |= JONJDBG_HAS_OPCODE_BP;

	jonjdbg_notice("Breakpoint #%d added at %s", new_break.id, name);
	JONJDBG_BREAK_MAPPING(new_break.id, &JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE]);
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_opline_ex(jonjdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	if (!zend_hash_index_exists(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], (zend_ulong) opline)) {
		jonjdbg_breakline_t new_break;

		JONJDBG_G(flags) |= JONJDBG_HAS_OPLINE_BP;

		JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_OPLINE);
		new_break.opline = (zend_ulong) opline;
		new_break.base = NULL;

		zend_hash_index_update(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE],
			(zend_ulong) opline, &new_break, sizeof(jonjdbg_breakline_t), NULL);

		jonjdbg_notice("Breakpoint #%d added at %#lx",
			new_break.id, new_break.opline);
		JONJDBG_BREAK_MAPPING(new_break.id, &JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]);
	}
} /* }}} */

static inline void jonjdbg_create_conditional_break(jonjdbg_breakcond_t *brake, const jonjdbg_param_t *param, const char *expr, size_t expr_len, zend_ulong hash TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakcond_t new_break;
	zend_uint cops = CG(compiler_options);
	zval pv;

	JONJDBG_BREAK_INIT(new_break, JONJDBG_BREAK_COND);
	new_break.hash = hash;

	if (param) {
		new_break.paramed = 1;
		jonjdbg_copy_param(
			param, &new_break.param TSRMLS_CC);
	} else {
		new_break.paramed = 0;
	}

	cops = CG(compiler_options);

	CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;

	new_break.code = estrndup(expr, expr_len);
	new_break.code_len = expr_len;

	Z_STRLEN(pv) = expr_len + sizeof("return ;") - 1;
	Z_STRVAL(pv) = emalloc(Z_STRLEN(pv) + 1);
	memcpy(Z_STRVAL(pv), "return ", sizeof("return ") - 1);
	memcpy(Z_STRVAL(pv) + sizeof("return ") - 1, expr, expr_len);
	Z_STRVAL(pv)[Z_STRLEN(pv) - 1] = ';';
	Z_STRVAL(pv)[Z_STRLEN(pv)] = '\0';
	Z_TYPE(pv) = IS_STRING;

	new_break.ops = zend_compile_string(
		&pv, "Conditional Breakpoint Code" TSRMLS_CC);

	zval_dtor(&pv);

	if (new_break.ops) {
		zend_hash_index_update(
			&JONJDBG_G(bp)[JONJDBG_BREAK_COND], hash, &new_break,
			sizeof(jonjdbg_breakcond_t), (void**)&brake);

		jonjdbg_notice("Conditional breakpoint #%d added %s/%p",
			brake->id, brake->code, brake->ops);

		JONJDBG_G(flags) |= JONJDBG_HAS_COND_BP;
		JONJDBG_BREAK_MAPPING(new_break.id, &JONJDBG_G(bp)[JONJDBG_BREAK_COND]);
	} else {
		 jonjdbg_error(
			"Failed to compile code for expression %s", expr);
		 efree((char*)new_break.code);
		 JONJDBG_G(bp_count)--;
	}
	CG(compiler_options) = cops;
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_expression(const char *expr, size_t expr_len TSRMLS_DC) /* {{{ */
{
	zend_ulong expr_hash = zend_inline_hash_func(expr, expr_len);
	jonjdbg_breakcond_t new_break;

	if (!zend_hash_index_exists(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], expr_hash)) {
		jonjdbg_create_conditional_break(
			&new_break, NULL, expr, expr_len, expr_hash TSRMLS_CC);
	} else {
		jonjdbg_notice("Conditional break %s exists", expr);
	}
} /* }}} */

JONJDBG_API void jonjdbg_set_breakpoint_at(const jonjdbg_param_t *param TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakcond_t new_break;
	jonjdbg_param_t *condition;
	zend_ulong hash = 0L;
	
	if (param->next) {
		condition = param->next;
		hash = zend_inline_hash_func(condition->str, condition->len);
		
		if (!zend_hash_index_exists(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], hash)) {
			jonjdbg_create_conditional_break(
				&new_break, param, 
					condition->str, condition->len, hash TSRMLS_CC);
		} else {
			jonjdbg_notice(
				"Conditional break %s exists at the specified location", condition->str);
		}	
	}
	
} /* }}} */

static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_file(zend_op_array *op_array TSRMLS_DC) /* {{{ */
{
	HashTable *breaks;
	jonjdbg_breakbase_t *brake;
	size_t name_len = strlen(op_array->filename);

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE], op_array->filename,
		name_len, (void**)&breaks) == FAILURE) {
		return NULL;
	}

	if (zend_hash_index_find(breaks, (*EG(opline_ptr))->lineno, (void**)&brake) == SUCCESS) {
		return brake;
	}

	return NULL;
} /* }}} */

static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_symbol(zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const char *fname;
	zend_op_array *ops;
	jonjdbg_breakbase_t *brake;

	if (fbc->type != ZEND_USER_FUNCTION) {
		return NULL;
	}

	ops = (zend_op_array*)fbc;

	if (ops->scope) {
		/* find method breaks here */
		return jonjdbg_find_breakpoint_method(ops TSRMLS_CC);
	}

	fname = ops->function_name;

	if (!fname) {
		fname = "main";
	}

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], fname, strlen(fname), (void**)&brake) == SUCCESS) {
		return brake;
	}

	return NULL;
} /* }}} */

static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_method(zend_op_array *ops TSRMLS_DC) /* {{{ */
{
	HashTable *class_table;
	jonjdbg_breakbase_t *brake;

	if (zend_hash_find(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD], ops->scope->name,
		ops->scope->name_length, (void**)&class_table) == SUCCESS) {
		char *lcname = zend_str_tolower_dup(ops->function_name, strlen(ops->function_name));
		size_t lcname_len = strlen(lcname);

		if (zend_hash_find(
		        class_table,
		        lcname,
		        lcname_len, (void**)&brake) == SUCCESS) {
			efree(lcname);
			return brake;
		}

		efree(lcname);
	}

	return NULL;
} /* }}} */

static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_opline(jonjdbg_opline_ptr_t opline TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakline_t *brake;

	if (zend_hash_index_find(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE],
		(zend_ulong) opline, (void**)&brake) == SUCCESS) {
		return (brake->base?(jonjdbg_breakbase_t *)brake->base:(jonjdbg_breakbase_t *)brake);
	}

	return NULL;
} /* }}} */

static inline jonjdbg_breakbase_t *jonjdbg_find_breakpoint_opcode(zend_uchar opcode TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakbase_t *brake;
	const char *opname = jonjdbg_decode_opcode(opcode);

	if (memcmp(opname, JONJDBG_STRL("UNKNOWN")) == 0) {
		return NULL;
	}

	if (zend_hash_index_find(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE],
		zend_hash_func(opname, strlen(opname)), (void**)&brake) == SUCCESS) {
		return brake;
	}
	return NULL;
} /* }}} */

static inline zend_bool jonjdbg_find_breakpoint_param(jonjdbg_param_t *param, zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
	zend_function *function = (zend_function*) execute_data->function_state.function;

	switch (param->type) {
		case NUMERIC_FUNCTION_PARAM:
		case STR_PARAM: {
			/* function breakpoint */

			if (function->type != ZEND_USER_FUNCTION) {
				return 0;
			}

			{
				const char *str = NULL;
				size_t len = 0L;
				zend_op_array *ops = (zend_op_array*)function;
				str = ops->function_name ? ops->function_name : "main";
				len = strlen(str);

				if (len == param->len && memcmp(param->str, str, len) == SUCCESS) {
					return param->type == STR_PARAM || execute_data->opline - ops->opcodes == param->num;
				}
			}
		} break;

		case FILE_PARAM: {
			if (param->file.line == zend_get_executed_lineno(TSRMLS_C)) {
				const char *str = zend_get_executed_filename(TSRMLS_C);
				size_t lengths[2] = {strlen(param->file.name), strlen(str)};

				if (lengths[0] == lengths[1]) {
					return (memcmp(
						param->file.name, str, lengths[0]) == SUCCESS);
				}
			}
		} break;

		case NUMERIC_METHOD_PARAM:
		case METHOD_PARAM: {
			if (function->type != ZEND_USER_FUNCTION) {
				return 0;
			}

			{
				zend_op_array *ops = (zend_op_array*) function;

				if (ops->scope) {
					size_t lengths[2] = {strlen(param->method.class), ops->scope->name_length};
					if (lengths[0] == lengths[1] && memcmp(param->method.class, ops->scope->name, lengths[0]) == SUCCESS) {
						lengths[0] = strlen(param->method.name);
						lengths[1] = strlen(ops->function_name);

						if (lengths[0] == lengths[1] && memcmp(param->method.name, ops->function_name, lengths[0]) == SUCCESS) {
							return param->type == METHOD_PARAM || (execute_data->opline - ops->opcodes) == param->num;
						}
					}
				}
			}
		} break;

		case ADDR_PARAM: {
			return ((zend_ulong)(jonjdbg_opline_ptr_t)execute_data->opline == param->addr);
		} break;

		default: {
			/* do nothing */
		} break;
	}
	return 0;
} /* }}} */

static inline jonjdbg_breakbase_t *jonjdbg_find_conditional_breakpoint(zend_execute_data *execute_data TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakcond_t *bp;
	HashPosition position;
	int breakpoint = FAILURE;

	for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], &position);
	     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], (void*)&bp, &position) == SUCCESS;
	     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], &position)) {
		zval *retval = NULL;
		int orig_interactive = CG(interactive);
		zval **orig_retval = EG(return_value_ptr_ptr);
		zend_op_array *orig_ops = EG(active_op_array);
		zend_op **orig_opline = EG(opline_ptr);

		if (((jonjdbg_breakbase_t*)bp)->disabled) {
			continue;
		}

		if (bp->paramed) {
			if (!jonjdbg_find_breakpoint_param(&bp->param, execute_data TSRMLS_CC)) {
				continue;
			}
		}

		ALLOC_INIT_ZVAL(retval);

		EG(return_value_ptr_ptr) = &retval;
		EG(active_op_array) = bp->ops;
		EG(no_extensions) = 1;

		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}

		CG(interactive) = 0;

		zend_try {
			JONJDBG_G(flags) |= JONJDBG_IN_COND_BP;
			zend_execute(EG(active_op_array) TSRMLS_CC);
#if JONJ_VERSION_ID >= 50700
			if (zend_is_true(retval TSRMLS_CC)) {
#else
			if (zend_is_true(retval)) {
#endif
				breakpoint = SUCCESS;
			}
		} zend_catch {
			CG(interactive) = orig_interactive;

			EG(no_extensions)=1;
			EG(return_value_ptr_ptr) = orig_retval;
			EG(active_op_array) = orig_ops;
			EG(opline_ptr) = orig_opline;
			JONJDBG_G(flags) &= ~JONJDBG_IN_COND_BP;
 		} zend_end_try();

		CG(interactive) = orig_interactive;

		EG(no_extensions)=1;
		EG(return_value_ptr_ptr) = orig_retval;
		EG(active_op_array) = orig_ops;
		EG(opline_ptr) = orig_opline;
		JONJDBG_G(flags) &= ~JONJDBG_IN_COND_BP;

		if (breakpoint == SUCCESS) {
			break;
		}
	}

	return (breakpoint == SUCCESS) ? ((jonjdbg_breakbase_t*)bp) : NULL;
} /* }}} */

JONJDBG_API jonjdbg_breakbase_t *jonjdbg_find_breakpoint(zend_execute_data* execute_data TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakbase_t *base = NULL;

	if (!(JONJDBG_G(flags) & JONJDBG_IS_BP_ENABLED)) {
		return NULL;
	}

	/* conditions cannot be executed by eval()'d code */
	if (!(JONJDBG_G(flags) & JONJDBG_IN_EVAL) &&
		(JONJDBG_G(flags) & JONJDBG_HAS_COND_BP) &&
		(base = jonjdbg_find_conditional_breakpoint(execute_data TSRMLS_CC))) {
		goto result;
	}

	if ((JONJDBG_G(flags) & JONJDBG_HAS_FILE_BP) &&
		(base = jonjdbg_find_breakpoint_file(execute_data->op_array TSRMLS_CC))) {
		goto result;
	}

	if (JONJDBG_G(flags) & (JONJDBG_HAS_METHOD_BP|JONJDBG_HAS_SYM_BP)) {
		/* check we are at the beginning of the stack */
		if (execute_data->opline == EG(active_op_array)->opcodes) {
			if ((base = jonjdbg_find_breakpoint_symbol(
					execute_data->function_state.function TSRMLS_CC))) {
				goto result;
			}
		}
	}

	if ((JONJDBG_G(flags) & JONJDBG_HAS_OPLINE_BP) &&
		(base = jonjdbg_find_breakpoint_opline(execute_data->opline TSRMLS_CC))) {
		goto result;
	}

	if ((JONJDBG_G(flags) & JONJDBG_HAS_OPCODE_BP) &&
		(base = jonjdbg_find_breakpoint_opcode(execute_data->opline->opcode TSRMLS_CC))) {
		goto result;
	}

	return NULL;

result:
	/* we return nothing for disable breakpoints */
	if (base->disabled) {
		return NULL;
	}

	return base;
} /* }}} */

JONJDBG_API void jonjdbg_delete_breakpoint(zend_ulong num TSRMLS_DC) /* {{{ */
{
	HashTable **table;
	HashPosition position;
	jonjdbg_breakbase_t *brake;

	if ((brake = jonjdbg_find_breakbase_ex(num, &table, &position TSRMLS_CC))) {
		char *key;
		zend_uint klen;
		zend_ulong idx;
		int type = brake->type;
		char *name = NULL;
		size_t name_len = 0L;

		switch (type) {
			case JONJDBG_BREAK_FILE:
			case JONJDBG_BREAK_METHOD:
				if (zend_hash_num_elements((*table)) == 1) {
					name = estrdup(brake->name);
					name_len = strlen(name);
					if (zend_hash_num_elements(&JONJDBG_G(bp)[type]) == 1) {
						JONJDBG_G(flags) &= ~(1<<(brake->type+1));
					}
				}
			break;

			default: {
				if (zend_hash_num_elements((*table)) == 1) {
					JONJDBG_G(flags) &= ~(1<<(brake->type+1));
				}
			}
		}

		switch (type) {
			case JONJDBG_BREAK_FILE_OPLINE:
			case JONJDBG_BREAK_FUNCTION_OPLINE:
			case JONJDBG_BREAK_METHOD_OPLINE:
				if (zend_hash_num_elements(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]) == 1) {
					JONJDBG_G(flags) &= JONJDBG_HAS_OPLINE_BP;
				}
				zend_hash_index_del(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], ((jonjdbg_breakopline_t*)brake)->opline);
		}

		switch (zend_hash_get_current_key_ex(
			(*table), &key, &klen, &idx, 0, &position)) {

			case HASH_KEY_IS_STRING:
				zend_hash_del((*table), key, klen);
			break;

			default:
				zend_hash_index_del((*table), idx);
		}

		switch (type) {
			case JONJDBG_BREAK_FILE:
			case JONJDBG_BREAK_METHOD:
				if (name) {
					zend_hash_del(&JONJDBG_G(bp)[type], name, name_len);
					efree(name);
				}
			break;
		}

		jonjdbg_notice("Deleted breakpoint #%ld", num);
		JONJDBG_BREAK_UNMAPPING(num);
	} else {
		jonjdbg_error("Failed to find breakpoint #%ld", num);
	}
} /* }}} */

JONJDBG_API void jonjdbg_clear_breakpoints(TSRMLS_D) /* {{{ */
{
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_COND]);
	zend_hash_clean(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP]);

	JONJDBG_G(flags) &= ~JONJDBG_BP_MASK;

	JONJDBG_G(bp_count) = 0;
} /* }}} */

JONJDBG_API void jonjdbg_hit_breakpoint(jonjdbg_breakbase_t *brake, zend_bool output TSRMLS_DC) /* {{{ */
{
	brake->hits++;

	if (output) {
		jonjdbg_print_breakpoint(brake TSRMLS_CC);
	}
} /* }}} */

JONJDBG_API void jonjdbg_print_breakpoint(jonjdbg_breakbase_t *brake TSRMLS_DC) /* {{{ */
{
	if (!brake)
		goto unknown;

	switch (brake->type) {
		case JONJDBG_BREAK_FILE: {
			jonjdbg_notice("Breakpoint #%d at %s:%ld, hits: %lu",
				((jonjdbg_breakfile_t*)brake)->id,
				((jonjdbg_breakfile_t*)brake)->filename,
				((jonjdbg_breakfile_t*)brake)->line,
				((jonjdbg_breakfile_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_SYM: {
			jonjdbg_notice("Breakpoint #%d in %s() at %s:%u, hits: %lu",
				((jonjdbg_breaksymbol_t*)brake)->id,
				((jonjdbg_breaksymbol_t*)brake)->symbol,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakfile_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_OPLINE: {
			jonjdbg_notice("Breakpoint #%d in %#lx at %s:%u, hits: %lu",
				((jonjdbg_breakline_t*)brake)->id,
				((jonjdbg_breakline_t*)brake)->opline,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakline_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_METHOD_OPLINE: {
			 jonjdbg_notice("Breakpoint #%d in %s::%s()#%lu at %s:%u, hits: %lu",
				((jonjdbg_breakopline_t*)brake)->id,
				((jonjdbg_breakopline_t*)brake)->class_name,
				((jonjdbg_breakopline_t*)brake)->func_name,
				((jonjdbg_breakopline_t*)brake)->opline_num,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakopline_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_FUNCTION_OPLINE: {
			 jonjdbg_notice("Breakpoint #%d in %s()#%lu at %s:%u, hits: %lu",
				((jonjdbg_breakopline_t*)brake)->id,
				((jonjdbg_breakopline_t*)brake)->func_name,
				((jonjdbg_breakopline_t*)brake)->opline_num,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakopline_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_FILE_OPLINE: {
			 jonjdbg_notice("Breakpoint #%d in %s:%lu at %s:%u, hits: %lu",
				((jonjdbg_breakopline_t*)brake)->id,
				((jonjdbg_breakopline_t*)brake)->class_name,
				((jonjdbg_breakopline_t*)brake)->opline_num,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakopline_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_OPCODE: {
			 jonjdbg_notice("Breakpoint #%d in %s at %s:%u, hits: %lu",
				((jonjdbg_breakop_t*)brake)->id,
				((jonjdbg_breakop_t*)brake)->name,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakop_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_METHOD: {
			 jonjdbg_notice("Breakpoint #%d in %s::%s() at %s:%u, hits: %lu",
				((jonjdbg_breakmethod_t*)brake)->id,
				((jonjdbg_breakmethod_t*)brake)->class_name,
				((jonjdbg_breakmethod_t*)brake)->func_name,
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C),
				((jonjdbg_breakmethod_t*)brake)->hits);
		} break;

		case JONJDBG_BREAK_COND: {
			if (((jonjdbg_breakcond_t*)brake)->paramed) {
				char *param;
				jonjdbg_notice("Conditional breakpoint #%d: at %s if %s %s:%u, hits: %lu",
					((jonjdbg_breakcond_t*)brake)->id,
					jonjdbg_param_tostring(&((jonjdbg_breakcond_t*)brake)->param, &param TSRMLS_CC),
					((jonjdbg_breakcond_t*)brake)->code,
					zend_get_executed_filename(TSRMLS_C),
					zend_get_executed_lineno(TSRMLS_C),
					((jonjdbg_breakcond_t*)brake)->hits);
				if (param)
					free(param);
			} else {
				jonjdbg_notice("Conditional breakpoint #%d: on %s == true %s:%u, hits: %lu",
					((jonjdbg_breakcond_t*)brake)->id,
					((jonjdbg_breakcond_t*)brake)->code,
					zend_get_executed_filename(TSRMLS_C),
					zend_get_executed_lineno(TSRMLS_C),
					((jonjdbg_breakcond_t*)brake)->hits);
			}

		} break;

		default: {
unknown:
			jonjdbg_notice("Unknown breakpoint at %s:%u",
				zend_get_executed_filename(TSRMLS_C),
				zend_get_executed_lineno(TSRMLS_C));
		}
	}
} /* }}} */

JONJDBG_API void jonjdbg_enable_breakpoint(zend_ulong id TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakbase_t *brake = jonjdbg_find_breakbase(id TSRMLS_CC);

	if (brake) {
		brake->disabled = 0;
	}
} /* }}} */

JONJDBG_API void jonjdbg_disable_breakpoint(zend_ulong id TSRMLS_DC) /* {{{ */
{
	jonjdbg_breakbase_t *brake = jonjdbg_find_breakbase(id TSRMLS_CC);

	if (brake) {
		brake->disabled = 1;
	}
} /* }}} */

JONJDBG_API void jonjdbg_enable_breakpoints(TSRMLS_D) /* {{{ */
{
	JONJDBG_G(flags) |= JONJDBG_IS_BP_ENABLED;
} /* }}} */

JONJDBG_API void jonjdbg_disable_breakpoints(TSRMLS_D) { /* {{{ */
	JONJDBG_G(flags) &= ~JONJDBG_IS_BP_ENABLED;
} /* }}} */

JONJDBG_API jonjdbg_breakbase_t *jonjdbg_find_breakbase(zend_ulong id TSRMLS_DC) /* {{{ */
{
	HashTable **table;
	HashPosition position;

	return jonjdbg_find_breakbase_ex(id, &table, &position TSRMLS_CC);
} /* }}} */

JONJDBG_API jonjdbg_breakbase_t *jonjdbg_find_breakbase_ex(zend_ulong id, HashTable ***table, HashPosition *position TSRMLS_DC) /* {{{ */
{
	if (zend_hash_index_find(&JONJDBG_G(bp)[JONJDBG_BREAK_MAP], id, (void**)table) == SUCCESS) {
		jonjdbg_breakbase_t *brake;

		for (zend_hash_internal_pointer_reset_ex((**table), position);
			zend_hash_get_current_data_ex((**table), (void**)&brake, position) == SUCCESS;
			zend_hash_move_forward_ex((**table), position)) {

			if (brake->id == id) {
				return brake;
			}
		}
	}
	return NULL;
} /* }}} */

JONJDBG_API void jonjdbg_print_breakpoints(zend_ulong type TSRMLS_DC) /* {{{ */
{
	switch (type) {
		case JONJDBG_BREAK_SYM: if ((JONJDBG_G(flags) & JONJDBG_HAS_SYM_BP)) {
			HashPosition position;
			jonjdbg_breaksymbol_t *brake;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Function Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], &position);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], (void**) &brake, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_SYM], &position)) {
				jonjdbg_writeln("#%d\t\t%s%s",
					brake->id, brake->symbol,
					((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
			}
		} break;

		case JONJDBG_BREAK_METHOD: if ((JONJDBG_G(flags) & JONJDBG_HAS_METHOD_BP)) {
			HashPosition position[2];
			HashTable *class_table;
			char *class_name = NULL;
			zend_uint class_len = 0;
			zend_ulong class_idx = 0L;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Method Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD], &position[0]);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD], (void**) &class_table, &position[0]) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD], &position[0])) {

				if (zend_hash_get_current_key_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD],
					&class_name, &class_len, &class_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {
					jonjdbg_breakmethod_t *brake;

					for (zend_hash_internal_pointer_reset_ex(class_table, &position[1]);
					     zend_hash_get_current_data_ex(class_table, (void**)&brake, &position[1]) == SUCCESS;
					     zend_hash_move_forward_ex(class_table, &position[1])) {
						jonjdbg_writeln("#%d\t\t%s::%s%s",
							brake->id, brake->class_name, brake->func_name,
							((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
					}
				}

			}
		} break;

		case JONJDBG_BREAK_FILE: if ((JONJDBG_G(flags) & JONJDBG_HAS_FILE_BP)) {
			HashPosition position[2];
			HashTable *points;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("File Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE], &position[0]);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE], (void**) &points, &position[0]) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE], &position[0])) {
				jonjdbg_breakfile_t *brake;

				for (zend_hash_internal_pointer_reset_ex(points, &position[1]);
				     zend_hash_get_current_data_ex(points, (void**)&brake, &position[1]) == SUCCESS;
				     zend_hash_move_forward_ex(points, &position[1])) {
					jonjdbg_writeln("#%d\t\t%s:%lu%s",
						brake->id, brake->filename, brake->line,
						((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
				}
			}

		} break;

		case JONJDBG_BREAK_OPLINE: if ((JONJDBG_G(flags) & JONJDBG_HAS_OPLINE_BP)) {
			HashPosition position;
			jonjdbg_breakline_t *brake;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Opline Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], &position);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], (void**) &brake, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_OPLINE], &position)) {
				switch (brake->type) {
					case JONJDBG_BREAK_METHOD_OPLINE:
					case JONJDBG_BREAK_FUNCTION_OPLINE:
					case JONJDBG_BREAK_FILE_OPLINE:
						jonjdbg_writeln("#%d\t\t%#lx\t\t(%s breakpoint)%s", brake->id, brake->opline,
							brake->type == JONJDBG_BREAK_METHOD_OPLINE?"method":
								brake->type == JONJDBG_BREAK_FUNCTION_OPLINE?"function":
									brake->type == JONJDBG_BREAK_FILE_OPLINE?"file":
										"--- error ---",
							((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

					default:
						jonjdbg_writeln("#%d\t\t%#lx", brake->id, brake->opline);
						break;
				}
			}
		} break;

		case JONJDBG_BREAK_METHOD_OPLINE: if ((JONJDBG_G(flags) & JONJDBG_HAS_METHOD_OPLINE_BP)) {
			HashPosition position[3];
			HashTable *class_table, *method_table;
			char *class_name = NULL, *method_name = NULL;
			zend_uint class_len = 0, method_len = 0;
			zend_ulong class_idx = 0L, method_idx = 0L;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Method opline Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE], &position[0]);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE], (void**) &class_table, &position[0]) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE], &position[0])) {

				if (zend_hash_get_current_key_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_METHOD_OPLINE],
					&class_name, &class_len, &class_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {

					for (zend_hash_internal_pointer_reset_ex(class_table, &position[1]);
					     zend_hash_get_current_data_ex(class_table, (void**) &method_table, &position[1]) == SUCCESS;
					     zend_hash_move_forward_ex(class_table, &position[1])) {

						if (zend_hash_get_current_key_ex(class_table,
							&method_name, &method_len, &method_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {

							jonjdbg_breakopline_t *brake;

							for (zend_hash_internal_pointer_reset_ex(method_table, &position[2]);
							     zend_hash_get_current_data_ex(method_table, (void**)&brake, &position[2]) == SUCCESS;
							     zend_hash_move_forward_ex(method_table, &position[2])) {
								jonjdbg_writeln("#%d\t\t%s::%s opline %ld%s",
									brake->id, brake->class_name, brake->func_name, brake->opline_num,
									((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
							}
						}
					}
				}

			}
		} break;

		case JONJDBG_BREAK_FUNCTION_OPLINE: if ((JONJDBG_G(flags) & JONJDBG_HAS_FUNCTION_OPLINE_BP)) {
			HashPosition position[2];
			HashTable *function_table;
			char *function_name = NULL;
			zend_uint function_len = 0;
			zend_ulong function_idx = 0L;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Function opline Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE], &position[0]);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE], (void**) &function_table, &position[0]) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE], &position[0])) {

				if (zend_hash_get_current_key_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FUNCTION_OPLINE],
					&function_name, &function_len, &function_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {

					jonjdbg_breakopline_t *brake;

					for (zend_hash_internal_pointer_reset_ex(function_table, &position[1]);
					     zend_hash_get_current_data_ex(function_table, (void**)&brake, &position[1]) == SUCCESS;
					     zend_hash_move_forward_ex(function_table, &position[1])) {
						jonjdbg_writeln("#%d\t\t%s opline %ld%s",
							brake->id, brake->func_name, brake->opline_num,
							((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
					}
				}

			}
		} break;

		case JONJDBG_BREAK_FILE_OPLINE: if ((JONJDBG_G(flags) & JONJDBG_HAS_FILE_OPLINE_BP)) {
			HashPosition position[2];
			HashTable *file_table;
			char *file_name = NULL;
			zend_uint file_len = 0;
			zend_ulong file_idx = 0L;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("File opline Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE], &position[0]);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE], (void**) &file_table, &position[0]) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE], &position[0])) {

				if (zend_hash_get_current_key_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_FILE_OPLINE],
					&file_name, &file_len, &file_idx, 0, &position[0]) == HASH_KEY_IS_STRING) {

					jonjdbg_breakopline_t *brake;

					for (zend_hash_internal_pointer_reset_ex(file_table, &position[1]);
					     zend_hash_get_current_data_ex(file_table, (void**)&brake, &position[1]) == SUCCESS;
					     zend_hash_move_forward_ex(file_table, &position[1])) {
						jonjdbg_writeln("#%d\t\t%s opline %ld%s",
							brake->id, brake->class_name, brake->opline_num,
							((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
					}
				}
			}
		} break;

		case JONJDBG_BREAK_COND: if ((JONJDBG_G(flags) & JONJDBG_HAS_COND_BP)) {
			HashPosition position;
			jonjdbg_breakcond_t *brake;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Conditional Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], &position);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], (void**) &brake, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_COND], &position)) {
				if (brake->paramed) {
					switch (brake->param.type) {
						case STR_PARAM:
							jonjdbg_writeln("#%d\t\tat %s if %s%s",
				 				brake->id,
				 				brake->param.str,
				 				brake->code,
				 				((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						case NUMERIC_FUNCTION_PARAM:
							jonjdbg_writeln("#%d\t\tat %s#%ld if %s%s",
				 				brake->id,
				 				brake->param.str,
								brake->param.num,
				 				brake->code,
				 				((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						case METHOD_PARAM:
							jonjdbg_writeln("#%d\t\tat %s::%s if %s%s",
				 				brake->id,
				 				brake->param.method.class,
				 				brake->param.method.name,
				 				brake->code,
				 				((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						case NUMERIC_METHOD_PARAM:
							jonjdbg_writeln("#%d\t\tat %s::%s#%ld if %s%s",
				 				brake->id,
				 				brake->param.method.class,
				 				brake->param.method.name,
								brake->param.num,
				 				brake->code,
				 				((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						case FILE_PARAM:
							jonjdbg_writeln("#%d\t\tat %s:%lu if %s%s",
				 				brake->id,
				 				brake->param.file.name,
				 				brake->param.file.line,
				 				brake->code,
				 				((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						case ADDR_PARAM:
							jonjdbg_writeln("#%d\t\tat #%lx if %s%s",
				 				brake->id,
				 				brake->param.addr,
				 				brake->code,
				 				((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						default:
							jonjdbg_error("Invalid parameter type for conditional breakpoint");
						return;
					}
				} else {
					jonjdbg_writeln("#%d\t\tif %s%s",
				 		brake->id, brake->code,
				 		((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
				}
			}
		} break;

		case JONJDBG_BREAK_OPCODE: if (JONJDBG_G(flags) & JONJDBG_HAS_OPCODE_BP) {
			HashPosition position;
			jonjdbg_breakop_t *brake;

			jonjdbg_writeln(SEPARATE);
			jonjdbg_writeln("Opcode Breakpoints:");
			for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE], &position);
			     zend_hash_get_current_data_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE], (void**) &brake, &position) == SUCCESS;
			     zend_hash_move_forward_ex(&JONJDBG_G(bp)[JONJDBG_BREAK_OPCODE], &position)) {
				jonjdbg_writeln("#%d\t\t%s%s",
					brake->id, brake->name,
					((jonjdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
			}
		} break;
	}
} /* }}} */
