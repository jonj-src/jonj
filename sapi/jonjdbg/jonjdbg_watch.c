/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The JONJ Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,	  |
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
#include "jonjdbg_btree.h"
#include "jonjdbg_watch.h"
#include "jonjdbg_utils.h"
#ifndef _WIN32
# include <unistd.h>
# include <sys/mman.h>
#endif

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);


typedef struct {
	void *page;
	size_t size;
	char reenable_writing;
	/* data must be last element */
	void *data;
} jonjdbg_watch_memdump;

#define MEMDUMP_SIZE(size) (sizeof(jonjdbg_watch_memdump) - sizeof(void *) + (size))


static jonjdbg_watchpoint_t *jonjdbg_check_for_watchpoint(void *addr TSRMLS_DC) {
	jonjdbg_watchpoint_t *watch;
	jonjdbg_btree_result *result = jonjdbg_btree_find_closest(&JONJDBG_G(watchpoint_tree), (zend_ulong)jonjdbg_get_page_boundary(addr) + jonjdbg_pagesize - 1);

	if (result == NULL) {
		return NULL;
	}

	watch = result->ptr;

	/* check if that addr is in a mprotect()'ed memory area */
	if ((char *)jonjdbg_get_page_boundary(watch->addr.ptr) > (char *)addr || (char *)jonjdbg_get_page_boundary(watch->addr.ptr) + jonjdbg_get_total_page_size(watch->addr.ptr, watch->size) < (char *)addr) {
		/* failure */
		return NULL;
	}

	return watch;
}

static void jonjdbg_change_watchpoint_access(jonjdbg_watchpoint_t *watch, int access TSRMLS_DC) {
	int m;

	/* pagesize is assumed to be in the range of 2^x */
	m = mprotect(jonjdbg_get_page_boundary(watch->addr.ptr), jonjdbg_get_total_page_size(watch->addr.ptr, watch->size), access);
}

static inline void jonjdbg_activate_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	jonjdbg_change_watchpoint_access(watch, PROT_READ TSRMLS_CC);
}

static inline void jonjdbg_deactivate_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	jonjdbg_change_watchpoint_access(watch, PROT_READ | PROT_WRITE TSRMLS_CC);
}

static inline void jonjdbg_store_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	jonjdbg_btree_insert(&JONJDBG_G(watchpoint_tree), (zend_ulong)watch->addr.ptr, watch);
}

static inline void jonjdbg_remove_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	jonjdbg_btree_delete(&JONJDBG_G(watchpoint_tree), (zend_ulong)watch->addr.ptr);
}

void jonjdbg_create_addr_watchpoint(void *addr, size_t size, jonjdbg_watchpoint_t *watch) {
	watch->addr.ptr = addr;
	watch->size = size;
}

void jonjdbg_create_zval_watchpoint(zval *zv, jonjdbg_watchpoint_t *watch) {
	jonjdbg_create_addr_watchpoint(zv, sizeof(zval), watch);
	watch->type = WATCH_ON_ZVAL;
}

void jonjdbg_create_ht_watchpoint(HashTable *ht, jonjdbg_watchpoint_t *watch) {
	jonjdbg_create_addr_watchpoint(ht, sizeof(HashTable), watch);
	watch->type = WATCH_ON_HASHTABLE;
}

void jonjdbg_watch_HashTable_dtor(zval **ptr);

static int jonjdbg_create_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	watch->flags |= JONJDBG_WATCH_SIMPLE;

	jonjdbg_store_watchpoint(watch TSRMLS_CC);
	zend_hash_add(&JONJDBG_G(watchpoints), watch->str, watch->str_len, &watch, sizeof(jonjdbg_watchpoint_t *), NULL);

	if (watch->type == WATCH_ON_ZVAL) {
		jonjdbg_btree_insert(&JONJDBG_G(watch_HashTables), (zend_ulong)watch->parent_container, watch->parent_container->pDestructor);
		watch->parent_container->pDestructor = (dtor_func_t)jonjdbg_watch_HashTable_dtor;
	}

	jonjdbg_activate_watchpoint(watch TSRMLS_CC);

	return SUCCESS;
}

static int jonjdbg_create_array_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	HashTable *ht;

	switch (Z_TYPE_P(watch->addr.zv)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(watch->addr.zv);
			break;
		case IS_OBJECT:
			ht = Z_OBJPROP_P(watch->addr.zv);
			break;
		default:
			return FAILURE;
	}

	jonjdbg_create_ht_watchpoint(ht, watch);

	jonjdbg_create_watchpoint(watch TSRMLS_CC);

	return SUCCESS;
}

static char *jonjdbg_get_property_key(char *key) {
	if (*key != 0) {
		return key;
	}
	return strchr(key + 1, 0) + 1;
}

static int jonjdbg_create_recursive_watchpoint(jonjdbg_watchpoint_t *watch TSRMLS_DC) {
	HashTable *ht;

	if (watch->type != WATCH_ON_ZVAL) {
		return FAILURE;
	}

	watch->flags |= JONJDBG_WATCH_RECURSIVE;
	jonjdbg_create_watchpoint(watch TSRMLS_CC);

	switch (Z_TYPE_P(watch->addr.zv)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(watch->addr.zv);
			break;
		case IS_OBJECT:
			ht = Z_OBJPROP_P(watch->addr.zv);
			break;
		default:
			return SUCCESS;
	}

	{
		HashPosition position;
		zval **zv;
		zval key;

		for (zend_hash_internal_pointer_reset_ex(ht, &position);
		     zend_hash_get_current_data_ex(ht, (void **)&zv, &position) == SUCCESS;
		     zend_hash_move_forward_ex(ht, &position)) {
			jonjdbg_watchpoint_t *new_watch = emalloc(sizeof(jonjdbg_watchpoint_t));

			new_watch->flags = JONJDBG_WATCH_RECURSIVE;
			new_watch->parent = watch;
			new_watch->parent_container = ht;

			zend_hash_get_current_key_zval_ex(ht, &key, &position);
			if (Z_TYPE(key) == IS_STRING) {
				new_watch->name_in_parent = zend_strndup(Z_STRVAL(key), Z_STRLEN(key));
				new_watch->name_in_parent_len = Z_STRLEN(key);
			} else {
				new_watch->name_in_parent = NULL;
				new_watch->name_in_parent_len = asprintf(&new_watch->name_in_parent, "%ld", Z_LVAL(key));
			}

			new_watch->str = NULL;
			new_watch->str_len = asprintf(&new_watch->str, "%.*s%s%s%s", (int)watch->str_len, watch->str, Z_TYPE_P(watch->addr.zv) == IS_ARRAY?"[":"->", jonjdbg_get_property_key(new_watch->name_in_parent), Z_TYPE_P(watch->addr.zv) == IS_ARRAY?"]":"");

			jonjdbg_create_zval_watchpoint(*zv, new_watch);
			jonjdbg_create_recursive_watchpoint(new_watch TSRMLS_CC);
		}
	}

	{
		jonjdbg_watchpoint_t *new_watch = emalloc(sizeof(jonjdbg_watchpoint_t));

		new_watch->parent = watch;
		new_watch->parent_container = watch->parent_container;
		new_watch->name_in_parent = zend_strndup(watch->name_in_parent, watch->name_in_parent_len);
		new_watch->name_in_parent_len = watch->name_in_parent_len;
		new_watch->str = NULL;
		new_watch->str_len = asprintf(&new_watch->str, "%.*s[]", (int)watch->str_len, watch->str);
		new_watch->flags = JONJDBG_WATCH_RECURSIVE;

		jonjdbg_create_ht_watchpoint(ht, new_watch);
		jonjdbg_create_watchpoint(new_watch TSRMLS_CC);
	}

	return SUCCESS;
}

static int jonjdbg_delete_watchpoint_recursive(jonjdbg_watchpoint_t *watch, zend_bool user_request TSRMLS_DC) {
	if (watch->type == WATCH_ON_HASHTABLE || (watch->type == WATCH_ON_ZVAL && (Z_TYPE_P(watch->addr.zv) == IS_ARRAY || Z_TYPE_P(watch->addr.zv) == IS_OBJECT))) {
		HashTable *ht;
		jonjdbg_btree_result *result;

		if (watch->type == WATCH_ON_HASHTABLE && user_request) {
			HashPosition position;
			zval **zv;
			zval key;
			char *str;
			int str_len;
			jonjdbg_watchpoint_t **watchpoint;

			ht = watch->addr.ht;

			for (zend_hash_internal_pointer_reset_ex(ht, &position);
			     zend_hash_get_current_data_ex(ht, (void **)&zv, &position) == SUCCESS;
			     zend_hash_move_forward_ex(ht, &position)) {
				zend_hash_get_current_key_zval_ex(ht, &key, &position);
				str = NULL;
				if (Z_TYPE(key) == IS_STRING) {
					str_len = asprintf(&str, "%.*s%s%s%s", (int)watch->parent->str_len, watch->parent->str, Z_TYPE_P(watch->parent->addr.zv) == IS_ARRAY?"[":"->", jonjdbg_get_property_key(Z_STRVAL(key)), Z_TYPE_P(watch->parent->addr.zv) == IS_ARRAY?"]":"");
				} else {
					str_len = asprintf(&str, "%.*s%s%li%s", (int)watch->parent->str_len, watch->parent->str, Z_TYPE_P(watch->parent->addr.zv) == IS_ARRAY?"[":"->", Z_LVAL(key), Z_TYPE_P(watch->parent->addr.zv) == IS_ARRAY?"]":"");
				}

				if (zend_hash_find(&JONJDBG_G(watchpoints), str, str_len, (void **) &watchpoint) == SUCCESS) {
					jonjdbg_delete_watchpoint_recursive(*watchpoint, 1 TSRMLS_CC);
				}
			}
		} else {
			switch (Z_TYPE_P(watch->addr.zv)) {
				case IS_ARRAY:
					ht = Z_ARRVAL_P(watch->addr.zv);
					break;
				case IS_OBJECT:
					ht = Z_OBJPROP_P(watch->addr.zv);
					break;
			}

			if ((result = jonjdbg_btree_find(&JONJDBG_G(watchpoint_tree), (zend_ulong) ht))) {
				jonjdbg_delete_watchpoint_recursive((jonjdbg_watchpoint_t *) result->ptr, user_request TSRMLS_CC);
			}
		}
	}

	return zend_hash_del(&JONJDBG_G(watchpoints), watch->str, watch->str_len);
}

static int jonjdbg_delete_watchpoint(jonjdbg_watchpoint_t *tmp_watch TSRMLS_DC) {
	int ret;
	jonjdbg_watchpoint_t *watch;
	jonjdbg_btree_result *result;

	if ((result = jonjdbg_btree_find(&JONJDBG_G(watchpoint_tree), (zend_ulong)tmp_watch->addr.ptr)) == NULL) {
		return FAILURE;
	}

	watch = result->ptr;

	if (watch->flags & JONJDBG_WATCH_RECURSIVE) {
		ret = jonjdbg_delete_watchpoint_recursive(watch, 1 TSRMLS_CC);
	} else {
		ret = zend_hash_del(&JONJDBG_G(watchpoints), watch->str, watch->str_len);
	}

	free(tmp_watch->str);
	efree(tmp_watch);

	return ret;
}

static int jonjdbg_watchpoint_parse_input(char *input, size_t len, HashTable *parent, size_t i, int (*callback)(jonjdbg_watchpoint_t * TSRMLS_DC), zend_bool silent TSRMLS_DC) {
	int ret = FAILURE;
	zend_bool new_index = 1;
	char *last_index;
	int index_len = 0;
	zval **zv;

	if (len < 2 || *input != '$') {
		goto error;
	}

	while (i++ < len) {
		if (i == len) {
			new_index = 1;
		} else {
			switch (input[i]) {
				case '[':
					new_index = 1;
					break;
				case ']':
					break;
				case '>':
					if (last_index[index_len - 1] == '-') {
						new_index = 1;
						index_len--;
					}
					break;

				default:
					if (new_index) {
						last_index = input + i;
						new_index = 0;
					}
					if (input[i - 1] == ']') {
						goto error;
					}
					index_len++;
			}
		}

		if (new_index && index_len == 0) {
			HashPosition position;
			for (zend_hash_internal_pointer_reset_ex(parent, &position);
			     zend_hash_get_current_data_ex(parent, (void **)&zv, &position) == SUCCESS;
			     zend_hash_move_forward_ex(parent, &position)) {
				if (i == len || (i == len - 1 && input[len - 1] == ']')) {
					zval *key = emalloc(sizeof(zval));
					jonjdbg_watchpoint_t *watch = emalloc(sizeof(jonjdbg_watchpoint_t));
					watch->flags = 0;
					zend_hash_get_current_key_zval_ex(parent, key, &position);
					convert_to_string(key);
					watch->str = malloc(i + Z_STRLEN_P(key) + 2);
					watch->str_len = sprintf(watch->str, "%.*s%s%s", (int)i, input, jonjdbg_get_property_key(Z_STRVAL_P(key)), input[len - 1] == ']'?"]":"");
					efree(key);
					watch->name_in_parent = zend_strndup(last_index, index_len);
					watch->name_in_parent_len = index_len;
					watch->parent_container = parent;
					jonjdbg_create_zval_watchpoint(*zv, watch);

					ret = callback(watch TSRMLS_CC) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				} else if (Z_TYPE_PP(zv) == IS_OBJECT) {
					jonjdbg_watchpoint_parse_input(input, len, Z_OBJPROP_PP(zv), i, callback, silent TSRMLS_CC);
				} else if (Z_TYPE_PP(zv) == IS_ARRAY) {
					jonjdbg_watchpoint_parse_input(input, len, Z_ARRVAL_PP(zv), i, callback, silent TSRMLS_CC);
				} else {
					/* Ignore silently */
				}
			}
			return ret;
		} else if (new_index) {
			char last_chr = last_index[index_len];
			last_index[index_len] = 0;
			if (zend_symtable_find(parent, last_index, index_len + 1, (void **)&zv) == FAILURE) {
				if (!silent) {
					jonjdbg_error("%.*s is undefined", (int)i, input);
				}
				return FAILURE;
			}
			last_index[index_len] = last_chr;
			if (i == len) {
				jonjdbg_watchpoint_t *watch = emalloc(sizeof(jonjdbg_watchpoint_t));
				watch->flags = 0;
				watch->str = zend_strndup(input, len);
				watch->str_len = len;
				watch->name_in_parent = zend_strndup(last_index, index_len);
				watch->name_in_parent_len = index_len;
				watch->parent_container = parent;
				jonjdbg_create_zval_watchpoint(*zv, watch);

				ret = callback(watch TSRMLS_CC) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
			} else if (Z_TYPE_PP(zv) == IS_OBJECT) {
				parent = Z_OBJPROP_PP(zv);
			} else if (Z_TYPE_PP(zv) == IS_ARRAY) {
				parent = Z_ARRVAL_PP(zv);
			} else {
				jonjdbg_error("%.*s is nor an array nor an object", (int)i, input);
				return FAILURE;
			}
			index_len = 0;
		}
	}

	return ret;
	error:
		jonjdbg_error("Malformed input");
		return FAILURE;
}

static int jonjdbg_watchpoint_parse_symtables(char *input, size_t len, int (*callback)(jonjdbg_watchpoint_t * TSRMLS_DC) TSRMLS_DC) {
	if (EG(This) && len >= 5 && !memcmp("$this", input, 5)) {
		zend_hash_add(EG(active_symbol_table), "this", sizeof("this"), &EG(This), sizeof(zval *), NULL);
	}

	if (zend_is_auto_global(input, len TSRMLS_CC) && jonjdbg_watchpoint_parse_input(input, len, &EG(symbol_table), 0, callback, 1 TSRMLS_CC) != FAILURE) {
		return SUCCESS;
	}

	return jonjdbg_watchpoint_parse_input(input, len, EG(active_symbol_table), 0, callback, 0 TSRMLS_CC);
}

JONJDBG_WATCH(delete) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			if (jonjdbg_delete_var_watchpoint(param->str, param->len TSRMLS_CC) == FAILURE) {
				jonjdbg_error("Nothing was deleted, no corresponding watchpoint found");
			} else {
				jonjdbg_notice("Removed watchpoint %.*s", (int)param->len, param->str);
			}
			break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

JONJDBG_WATCH(recursive) /* {{{ */
{
	if (jonjdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			if (jonjdbg_watchpoint_parse_symtables(param->str, param->len, jonjdbg_create_recursive_watchpoint TSRMLS_CC) != FAILURE) {
				jonjdbg_notice("Set recursive watchpoint on %.*s", (int)param->len, param->str);
			}
			break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

JONJDBG_WATCH(array) /* {{{ */
{
	if (jonjdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			if (jonjdbg_watchpoint_parse_symtables(param->str, param->len, jonjdbg_create_array_watchpoint TSRMLS_CC) != FAILURE) {
				jonjdbg_notice("Set array watchpoint on %.*s", (int)param->len, param->str);
			}
			break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

void jonjdbg_watch_HashTable_dtor(zval **zv) {
	jonjdbg_btree_result *result;
	TSRMLS_FETCH();

	zval_ptr_dtor_wrapper(zv);

	if ((result = jonjdbg_btree_find(&JONJDBG_G(watchpoint_tree), (zend_ulong)*zv))) {
		jonjdbg_watchpoint_t *watch = result->ptr;

		JONJDBG_G(watchpoint_hit) = 1;

		jonjdbg_notice("%.*s was removed, removing watchpoint%s", (int)watch->str_len, watch->str, (watch->flags & JONJDBG_WATCH_RECURSIVE)?" recursively":"");

		if (watch->flags & JONJDBG_WATCH_RECURSIVE) {
			jonjdbg_delete_watchpoint_recursive(watch, 0 TSRMLS_CC);
		} else {
			zend_hash_del(&JONJDBG_G(watchpoints), watch->str, watch->str_len);
		}
	}
}


int jonjdbg_create_var_watchpoint(char *input, size_t len TSRMLS_DC) {
	if (jonjdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}

	return jonjdbg_watchpoint_parse_symtables(input, len, jonjdbg_create_watchpoint TSRMLS_CC);
}

int jonjdbg_delete_var_watchpoint(char *input, size_t len TSRMLS_DC) {
	if (jonjdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}

	return jonjdbg_watchpoint_parse_symtables(input, len, jonjdbg_delete_watchpoint TSRMLS_CC);
}

#ifdef _WIN32
int jonjdbg_watchpoint_segfault_handler(void *addr TSRMLS_DC) {
#else
int jonjdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC) {
#endif
	void *page;
	jonjdbg_watch_memdump *dump;
	jonjdbg_watchpoint_t *watch;
	size_t size;

	watch = jonjdbg_check_for_watchpoint(
#ifdef _WIN32
		addr
#else
		info->si_addr
#endif
		TSRMLS_CC);

	if (watch == NULL) {
		return FAILURE;
	}

	page = jonjdbg_get_page_boundary(watch->addr.ptr);
	size = jonjdbg_get_total_page_size(watch->addr.ptr, watch->size);

	/* re-enable writing */
	mprotect(page, size, PROT_READ | PROT_WRITE);

	dump = malloc(MEMDUMP_SIZE(size));
	dump->page = page;
	dump->size = size;

	memcpy(&dump->data, page, size);

	zend_llist_add_element(&JONJDBG_G(watchlist_mem), &dump);

	return SUCCESS;
}

void jonjdbg_watchpoints_clean(TSRMLS_D) {
	zend_hash_clean(&JONJDBG_G(watchpoints));
}

static void jonjdbg_watch_dtor(void *pDest) {
	jonjdbg_watchpoint_t *watch = *(jonjdbg_watchpoint_t **)pDest;
	TSRMLS_FETCH();

	jonjdbg_deactivate_watchpoint(watch TSRMLS_CC);
	jonjdbg_remove_watchpoint(watch TSRMLS_CC);

	free(watch->str);
	free(watch->name_in_parent);
	efree(watch);
}

static void jonjdbg_watch_mem_dtor(void *llist_data) {
	jonjdbg_watch_memdump *dump = *(jonjdbg_watch_memdump **)llist_data;

	/* Disble writing again */
	if (dump->reenable_writing) {
		mprotect(dump->page, dump->size, PROT_READ);
	}

	free(*(void **)llist_data);
}

void jonjdbg_setup_watchpoints(TSRMLS_D) {
#if _SC_PAGE_SIZE
	jonjdbg_pagesize = sysconf(_SC_PAGE_SIZE);
#elif _SC_PAGESIZE
	jonjdbg_pagesize = sysconf(_SC_PAGESIZE);
#elif _SC_NUTC_OS_PAGESIZE
	jonjdbg_pagesize = sysconf(_SC_NUTC_OS_PAGESIZE);
#else
	jonjdbg_pagesize = 4096; /* common pagesize */
#endif

	zend_llist_init(&JONJDBG_G(watchlist_mem), sizeof(void *), jonjdbg_watch_mem_dtor, 1);
	jonjdbg_btree_init(&JONJDBG_G(watchpoint_tree), sizeof(void *) * 8);
	jonjdbg_btree_init(&JONJDBG_G(watch_HashTables), sizeof(void *) * 8);
	zend_hash_init(&JONJDBG_G(watchpoints), 8, NULL, jonjdbg_watch_dtor, 0 ZEND_FILE_LINE_CC);
}

static void jonjdbg_print_changed_zval(jonjdbg_watch_memdump *dump TSRMLS_DC) {
	/* fetch all changes between dump->page and dump->page + dump->size */
	jonjdbg_btree_position pos = jonjdbg_btree_find_between(&JONJDBG_G(watchpoint_tree), (zend_ulong)dump->page, (zend_ulong)dump->page + dump->size);
	jonjdbg_btree_result *result, *htresult;
	int elementDiff;
	void *curTest;

	dump->reenable_writing = 0;

	while ((result = jonjdbg_btree_next(&pos))) {
		jonjdbg_watchpoint_t *watch = result->ptr, *htwatch;
		void *oldPtr = (char *)&dump->data + ((size_t)watch->addr.ptr - (size_t)dump->page);
		char reenable = 1;

		if ((size_t)watch->addr.ptr < (size_t)dump->page || (size_t)watch->addr.ptr + watch->size > (size_t)dump->page + dump->size) {
			continue;
		}

		/* Test if the zval was separated and if necessary move the watchpoint */
		if (zend_hash_find(watch->parent_container, watch->name_in_parent, watch->name_in_parent_len + 1, &curTest) == SUCCESS) {
			if (watch->type == WATCH_ON_HASHTABLE) {
				switch (Z_TYPE_PP((zval **)curTest)) {
					case IS_ARRAY:
						curTest = (void *)Z_ARRVAL_PP((zval **)curTest);
						break;
					case IS_OBJECT:
						curTest = (void *)Z_OBJPROP_PP((zval **)curTest);
						break;
				}
			} else {
				curTest = *(void **)curTest;
			}

			if (curTest != watch->addr.ptr) {
				jonjdbg_deactivate_watchpoint(watch TSRMLS_CC);
				jonjdbg_remove_watchpoint(watch TSRMLS_CC);
				watch->addr.ptr = curTest;
				jonjdbg_store_watchpoint(watch TSRMLS_CC);
				jonjdbg_activate_watchpoint(watch TSRMLS_CC);

				reenable = 0;
			}
		}

		/* Show to the user what changed and delete watchpoint upon removal */
		if (memcmp(oldPtr, watch->addr.ptr, watch->size) != SUCCESS) {
			if (JONJDBG_G(flags) & JONJDBG_SHOW_REFCOUNTS || (watch->type == WATCH_ON_ZVAL && memcmp(oldPtr, watch->addr.zv, sizeof(zvalue_value))) || (watch->type == WATCH_ON_HASHTABLE
#if ZEND_DEBUG
			    && !watch->addr.ht->inconsistent
#endif
			    && zend_hash_num_elements((HashTable *)oldPtr) != zend_hash_num_elements(watch->addr.ht))) {
				JONJDBG_G(watchpoint_hit) = 1;

				jonjdbg_notice("Breaking on watchpoint %s", watch->str);
			}

			switch (watch->type) {
				case WATCH_ON_ZVAL: {
					int removed = ((zval *)oldPtr)->refcount__gc != watch->addr.zv->refcount__gc && !zend_symtable_exists(watch->parent_container, watch->name_in_parent, watch->name_in_parent_len + 1);
					int show_value = memcmp(oldPtr, watch->addr.zv, sizeof(zvalue_value));
					int show_ref = ((zval *)oldPtr)->refcount__gc != watch->addr.zv->refcount__gc || ((zval *)oldPtr)->is_ref__gc != watch->addr.zv->is_ref__gc;

					if (removed || show_value) {
						jonjdbg_write("Old value: ");
						if ((Z_TYPE_P((zval *)oldPtr) == IS_ARRAY || Z_TYPE_P((zval *)oldPtr) == IS_OBJECT) && removed) {
							jonjdbg_writeln("Value inaccessible, HashTable already destroyed");
						} else {
							zend_print_flat_zval_r((zval *)oldPtr TSRMLS_CC);
							jonjdbg_writeln("");
						}
					}
					if (JONJDBG_G(flags) & JONJDBG_SHOW_REFCOUNTS && (removed || show_ref)) {
						jonjdbg_writeln("Old refcount: %d; Old is_ref: %d", ((zval *)oldPtr)->refcount__gc, ((zval *)oldPtr)->is_ref__gc);
					}

					/* check if zval was removed */
					if (removed) {
						jonjdbg_notice("Watchpoint %s was unset, removing watchpoint", watch->str);
						zend_hash_del(&JONJDBG_G(watchpoints), watch->str, watch->str_len);

						reenable = 0;

						if (Z_TYPE_P((zval *)oldPtr) == IS_ARRAY || Z_TYPE_P((zval *)oldPtr) == IS_OBJECT) {
							goto remove_ht_watch;
						}

						break;
					}

					if (show_value) {
						jonjdbg_write("New value: ");
						zend_print_flat_zval_r(watch->addr.zv TSRMLS_CC);
						jonjdbg_writeln("");
					}
					if (JONJDBG_G(flags) & JONJDBG_SHOW_REFCOUNTS && show_ref) {
						jonjdbg_writeln("New refcount: %d; New is_ref: %d", watch->addr.zv->refcount__gc, watch->addr.zv->is_ref__gc);
					}

					if ((Z_TYPE_P(watch->addr.zv) == IS_ARRAY && Z_ARRVAL_P(watch->addr.zv) != Z_ARRVAL_P((zval *)oldPtr)) || (Z_TYPE_P(watch->addr.zv) != IS_OBJECT && Z_OBJ_HANDLE_P(watch->addr.zv) == Z_OBJ_HANDLE_P((zval *)oldPtr))) {
						/* add new watchpoints if necessary */
						if (watch->flags & JONJDBG_WATCH_RECURSIVE) {
							jonjdbg_create_recursive_watchpoint(watch TSRMLS_CC);
						}
					}

					if ((Z_TYPE_P((zval *)oldPtr) != IS_ARRAY || Z_ARRVAL_P(watch->addr.zv) == Z_ARRVAL_P((zval *)oldPtr)) && (Z_TYPE_P((zval *)oldPtr) != IS_OBJECT || Z_OBJ_HANDLE_P(watch->addr.zv) == Z_OBJ_HANDLE_P((zval *)oldPtr))) {
						break;
					}

remove_ht_watch:
					if ((htresult = jonjdbg_btree_find(&JONJDBG_G(watchpoint_tree), (zend_ulong)Z_ARRVAL_P((zval *)oldPtr)))) {
						htwatch = htresult->ptr;
						zend_hash_del(&JONJDBG_G(watchpoints), htwatch->str, htwatch->str_len);
					}

					break;
				}
				case WATCH_ON_HASHTABLE:

#if ZEND_DEBUG
					if (watch->addr.ht->inconsistent) {
						jonjdbg_notice("Watchpoint %s was unset, removing watchpoint", watch->str);
						zend_hash_del(&JONJDBG_G(watchpoints), watch->str, watch->str_len);

						reenable = 0;

						break;
					}
#endif

					elementDiff = zend_hash_num_elements((HashTable *)oldPtr) - zend_hash_num_elements(watch->addr.ht);
					if (elementDiff) {
						if (elementDiff > 0) {
							jonjdbg_writeln("%d elements were removed from the array", elementDiff);
						} else {
							jonjdbg_writeln("%d elements were added to the array", -elementDiff);

							/* add new watchpoints if necessary */
							if (watch->flags & JONJDBG_WATCH_RECURSIVE) {
								jonjdbg_create_recursive_watchpoint(watch TSRMLS_CC);
							}
						}
					}
					if (((HashTable *)oldPtr)->pInternalPointer != watch->addr.ht->pInternalPointer) {
						jonjdbg_writeln("Internal pointer of array was changed");
					}
					break;
			}
		}

		dump->reenable_writing = dump->reenable_writing | reenable;
	}
}

int jonjdbg_print_changed_zvals(TSRMLS_D) {
	zend_llist_position pos;
	jonjdbg_watch_memdump **dump;
	int ret;

	if (zend_llist_count(&JONJDBG_G(watchlist_mem)) == 0) {
		return FAILURE;
	}

	dump = (jonjdbg_watch_memdump **)zend_llist_get_last_ex(&JONJDBG_G(watchlist_mem), &pos);

	do {
		jonjdbg_print_changed_zval(*dump TSRMLS_CC);
	} while ((dump = (jonjdbg_watch_memdump **)zend_llist_get_prev_ex(&JONJDBG_G(watchlist_mem), &pos)));

	zend_llist_clean(&JONJDBG_G(watchlist_mem));

	ret = JONJDBG_G(watchpoint_hit)?SUCCESS:FAILURE;
	JONJDBG_G(watchpoint_hit) = 0;

	return ret;
}

void jonjdbg_list_watchpoints(TSRMLS_D) {
	HashPosition position;
	jonjdbg_watchpoint_t **watch;

	for (zend_hash_internal_pointer_reset_ex(&JONJDBG_G(watchpoints), &position);
	     zend_hash_get_current_data_ex(&JONJDBG_G(watchpoints), (void**) &watch, &position) == SUCCESS;
	     zend_hash_move_forward_ex(&JONJDBG_G(watchpoints), &position)) {
		jonjdbg_writeln("%.*s", (int)(*watch)->str_len, (*watch)->str);
	}
}

void jonjdbg_watch_efree(void *ptr) {
	jonjdbg_btree_result *result;
	TSRMLS_FETCH();

	result = jonjdbg_btree_find_closest(&JONJDBG_G(watchpoint_tree), (zend_ulong)ptr);

	if (result) {
		jonjdbg_watchpoint_t *watch = result->ptr;

		if ((size_t)watch->addr.ptr + watch->size > (size_t)ptr) {
			zend_hash_del(&JONJDBG_G(watchpoints), watch->str, watch->str_len);
		}
	}

	JONJDBG_G(original_free_function)(ptr);
}
