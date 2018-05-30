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

#include "jonjdbg_btree.h"
#include "jonjdbg.h"

#define CHOOSE_BRANCH(n) \
	branch = branch->branches[!!(n)];

#ifdef _Win32
# define emalloc malloc
# define efree free
#endif

/* depth in bits */
void jonjdbg_btree_init(jonjdbg_btree *tree, zend_ulong depth) {
	tree->depth = depth;
	tree->branch = NULL;
	tree->count = 0;
}

jonjdbg_btree_result *jonjdbg_btree_find(jonjdbg_btree *tree, zend_ulong idx) {
	jonjdbg_btree_branch *branch = tree->branch;
	int i = tree->depth - 1;

	if (branch == NULL) {
		return NULL;
	}

	do {
		if ((idx >> i) % 2 == 1) {
		 	if (branch->branches[1]) {
				CHOOSE_BRANCH(1);
			} else {
				return NULL;
			}
		} else {
			if (branch->branches[0]) {
				CHOOSE_BRANCH(0);
			} else {
				return NULL;
			}
		}
	} while (i--);

	return &branch->result;
}

jonjdbg_btree_result *jonjdbg_btree_find_closest(jonjdbg_btree *tree, zend_ulong idx) {
	jonjdbg_btree_branch *branch = tree->branch;
	int i = tree->depth - 1, last_superior_i = -1;
	zend_bool had_alternative_branch = 0;

	if (branch == NULL) {
		return NULL;
	}

	/* find nearest watchpoint */
	do {
		/* an impossible branch was found if: */
		if (!had_alternative_branch && (idx >> i) % 2 == 0 && !branch->branches[0]) {
			/* there's no lower branch than idx */
			if (last_superior_i == -1) {
				/* failure */
				return NULL;
			}
			/* reset state */
			branch = tree->branch;
			i = tree->depth - 1;
			/* follow branch according to bits in idx until the last lower branch before the impossible branch */
			do {
				CHOOSE_BRANCH((idx >> i) % 2 == 1 && branch->branches[1]);
			} while (--i > last_superior_i);
			/* use now the lower branch of which we can be sure that it contains only branches lower than idx */
			CHOOSE_BRANCH(0);
			/* and choose the highest possible branch in the branch containing only branches lower than idx */
			while (i--) {
				CHOOSE_BRANCH(branch->branches[1]);
			}
			break;
		}
		/* follow branch according to bits in idx until having found an impossible branch */
		if (had_alternative_branch || (idx >> i) % 2 == 1) {
			if (branch->branches[1]) {
				if (branch->branches[0]) {
					last_superior_i = i;
				}
				CHOOSE_BRANCH(1);
			} else {
				CHOOSE_BRANCH(0);
				had_alternative_branch = 1;
			}
		} else {
			CHOOSE_BRANCH(0);
		}
	} while (i--);

	return &branch->result;
}

jonjdbg_btree_position jonjdbg_btree_find_between(jonjdbg_btree *tree, zend_ulong lower_idx, zend_ulong higher_idx) {
	jonjdbg_btree_position pos;

	pos.tree = tree;
	pos.end = lower_idx;
	pos.cur = higher_idx;

	return pos;
}

jonjdbg_btree_result *jonjdbg_btree_next(jonjdbg_btree_position *pos) {
	jonjdbg_btree_result *result = jonjdbg_btree_find_closest(pos->tree, pos->cur);

	if (result == NULL || result->idx < pos->end) {
		return NULL;
	}

	pos->cur = result->idx - 1;

	return result;
}

int jonjdbg_btree_insert_or_update(jonjdbg_btree *tree, zend_ulong idx, void *ptr, int flags) {
	int i = tree->depth - 1;
	jonjdbg_btree_branch **branch = &tree->branch;

	do {
		if (*branch == NULL) {
			break;
		}
		branch = &(*branch)->branches[(idx >> i) % 2];
	} while (i--);

	if (*branch == NULL) {
		if (!(flags & JONJDBG_BTREE_INSERT)) {
			return FAILURE;
		}

		{
			jonjdbg_btree_branch *memory = *branch = emalloc((i + 2) * sizeof(jonjdbg_btree_branch));
			do {
				(*branch)->branches[!((idx >> i) % 2)] = NULL;
				branch = &(*branch)->branches[(idx >> i) % 2];
				*branch = ++memory;
			} while (i--);
			tree->count++;
		}
	} else if (!(flags & JONJDBG_BTREE_UPDATE)) {
		return FAILURE;
	}

	(*branch)->result.idx = idx;
	(*branch)->result.ptr = ptr;

	return SUCCESS;
}

int jonjdbg_btree_delete(jonjdbg_btree *tree, zend_ulong idx) {
	int i = tree->depth;
	jonjdbg_btree_branch *branch = tree->branch;
	int i_last_dual_branch = -1, last_dual_branch_branch;
	jonjdbg_btree_branch *last_dual_branch = NULL;

	goto check_branch_existence;
	do {
		if (branch->branches[0] && branch->branches[1]) {
			last_dual_branch = branch;
			i_last_dual_branch = i;
			last_dual_branch_branch = (idx >> i) % 2;
		}
		branch = branch->branches[(idx >> i) % 2];

check_branch_existence:
		if (branch == NULL) {
			return FAILURE;
		}
	} while (i--);

	tree->count--;

	if (i_last_dual_branch == -1) {
		efree(tree->branch);
		tree->branch = NULL;
	} else {
		if (last_dual_branch->branches[last_dual_branch_branch] == last_dual_branch + 1) {
			jonjdbg_btree_branch *original_branch = last_dual_branch->branches[!last_dual_branch_branch];

			memcpy(last_dual_branch + 1, last_dual_branch->branches[!last_dual_branch_branch], (i_last_dual_branch + 1) * sizeof(jonjdbg_btree_branch));
			efree(last_dual_branch->branches[!last_dual_branch_branch]);
			last_dual_branch->branches[!last_dual_branch_branch] = last_dual_branch + 1;

			branch = last_dual_branch->branches[!last_dual_branch_branch];
			for (i = i_last_dual_branch; i--;) {
				branch = (branch->branches[branch->branches[1] == ++original_branch] = last_dual_branch + i_last_dual_branch - i + 1);
			}
		} else {
			efree(last_dual_branch->branches[last_dual_branch_branch]);
		}

		last_dual_branch->branches[last_dual_branch_branch] = NULL;
	}

	return SUCCESS;
}
