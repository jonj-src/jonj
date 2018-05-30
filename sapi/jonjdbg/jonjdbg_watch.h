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

#ifndef JONJDBG_WATCH_H
#define JONJDBG_WATCH_H

#include "TSRM.h"
#include "jonjdbg_cmd.h"

#ifdef _WIN32
# include "jonjdbg_win.h"
#endif

#define JONJDBG_WATCH(name) JONJDBG_COMMAND(watch_##name)

/**
 * Printer Forward Declarations
 */
JONJDBG_WATCH(array);
JONJDBG_WATCH(delete);
JONJDBG_WATCH(recursive);

/**
 * Commands
 */

static const jonjdbg_command_t jonjdbg_watch_commands[] = {
	JONJDBG_COMMAND_D_EX(array,      "create watchpoint on an array", 'a', watch_array,     NULL, "s"),
	JONJDBG_COMMAND_D_EX(delete,     "delete watchpoint",             'd', watch_delete,    NULL, "s"),
	JONJDBG_COMMAND_D_EX(recursive,  "create recursive watchpoints",  'r', watch_recursive, NULL, "s"),
	JONJDBG_END_COMMAND
};

/* Watchpoint functions/typedefs */

typedef enum {
	WATCH_ON_ZVAL,
	WATCH_ON_HASHTABLE,
} jonjdbg_watchtype;


#define JONJDBG_WATCH_SIMPLE	0x0
#define JONJDBG_WATCH_RECURSIVE	0x1

typedef struct _jonjdbg_watchpoint_t jonjdbg_watchpoint_t;

struct _jonjdbg_watchpoint_t {
	jonjdbg_watchpoint_t *parent;
	HashTable *parent_container;
	char *name_in_parent;
	size_t name_in_parent_len;
	char *str;
	size_t str_len;
	union {
		zval *zv;
		HashTable *ht;
		void *ptr;
	} addr;
	size_t size;
	jonjdbg_watchtype type;
	char flags;
};

void jonjdbg_setup_watchpoints(TSRMLS_D);

#ifndef _WIN32
int jonjdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC);
#else
int jonjdbg_watchpoint_segfault_handler(void *addr TSRMLS_DC);
#endif

void jonjdbg_create_addr_watchpoint(void *addr, size_t size, jonjdbg_watchpoint_t *watch);
void jonjdbg_create_zval_watchpoint(zval *zv, jonjdbg_watchpoint_t *watch);

int jonjdbg_delete_var_watchpoint(char *input, size_t len TSRMLS_DC);
int jonjdbg_create_var_watchpoint(char *input, size_t len TSRMLS_DC);

int jonjdbg_print_changed_zvals(TSRMLS_D);

void jonjdbg_list_watchpoints(TSRMLS_D);

void jonjdbg_watch_efree(void *ptr);


static long jonjdbg_pagesize;

static zend_always_inline void *jonjdbg_get_page_boundary(void *addr) {
	return (void *)((size_t)addr & ~(jonjdbg_pagesize - 1));
}

static zend_always_inline size_t jonjdbg_get_total_page_size(void *addr, size_t size) {
	return (size_t)jonjdbg_get_page_boundary((void *)((size_t)addr + size - 1)) - (size_t)jonjdbg_get_page_boundary(addr) + jonjdbg_pagesize;
}

#endif
