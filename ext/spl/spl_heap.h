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
   | Authors: Etienne Kneuss <colder@jonj.tk>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef SPL_HEAP_H
#define SPL_HEAP_H

#include "jonj.h"
#include "jonj_spl.h"

extern JONJAPI zend_class_entry *spl_ce_SplHeap;
extern JONJAPI zend_class_entry *spl_ce_SplMinHeap;
extern JONJAPI zend_class_entry *spl_ce_SplMaxHeap;

extern JONJAPI zend_class_entry *spl_ce_SplPriorityQueue;

JONJ_MINIT_FUNCTION(spl_heap);

#endif /* SPL_HEAP_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
