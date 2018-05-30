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

#ifndef JONJDBG_BTREE_H
#define JONJDBG_BTREE_H

#include "zend.h"

typedef struct {
	zend_ulong idx;
	void *ptr;
} jonjdbg_btree_result;

typedef union _jonjdbg_btree_branch jonjdbg_btree_branch;
union _jonjdbg_btree_branch {
	jonjdbg_btree_branch *branches[2];
	jonjdbg_btree_result result;
};

typedef struct {
	zend_ulong count;
	zend_ulong depth;
	jonjdbg_btree_branch *branch;
} jonjdbg_btree;

typedef struct {
	jonjdbg_btree *tree;
	zend_ulong cur;
	zend_ulong end;
} jonjdbg_btree_position;

void jonjdbg_btree_init(jonjdbg_btree *tree, zend_ulong depth);
jonjdbg_btree_result *jonjdbg_btree_find(jonjdbg_btree *tree, zend_ulong idx);
jonjdbg_btree_result *jonjdbg_btree_find_closest(jonjdbg_btree *tree, zend_ulong idx);
jonjdbg_btree_position jonjdbg_btree_find_between(jonjdbg_btree *tree, zend_ulong lower_idx, zend_ulong higher_idx);
jonjdbg_btree_result *jonjdbg_btree_next(jonjdbg_btree_position *pos);
int jonjdbg_btree_delete(jonjdbg_btree *tree, zend_ulong idx);

#define JONJDBG_BTREE_INSERT 1
#define JONJDBG_BTREE_UPDATE 2
#define JONJDBG_BTREE_OVERWRITE (JONJDBG_BTREE_INSERT | JONJDBG_BTREE_UPDATE)

int jonjdbg_btree_insert_or_update(jonjdbg_btree *tree, zend_ulong idx, void *ptr, int flags);
#define jonjdbg_btree_insert(tree, idx, ptr) jonjdbg_btree_insert_or_update(tree, idx, ptr, JONJDBG_BTREE_INSERT)
#define jonjdbg_btree_update(tree, idx, ptr) jonjdbg_btree_insert_or_update(tree, idx, ptr, JONJDBG_BTREE_UPDATE)
#define jonjdbg_btree_overwrite(tree, idx, ptr) jonjdbg_btree_insert_or_update(tree, idx, ptr, JONJDBG_BTREE_OWERWRITE)

#endif
