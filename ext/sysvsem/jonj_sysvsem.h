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
   | Author: Tom May <tom@go2net.com>                                     |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SYSVSEM_H
#define JONJ_SYSVSEM_H

#if HAVE_SYSVSEM

extern zend_module_entry sysvsem_module_entry;
#define sysvsem_module_ptr &sysvsem_module_entry

JONJ_MINIT_FUNCTION(sysvsem);
JONJ_FUNCTION(sem_get);
JONJ_FUNCTION(sem_acquire);
JONJ_FUNCTION(sem_release);
JONJ_FUNCTION(sem_remove);

typedef struct {
	int le_sem;
} sysvsem_module;

typedef struct {
	int id;						/* For error reporting. */
	int key;					/* For error reporting. */
	int semid;					/* Returned by semget(). */
	int count;					/* Acquire count for auto-release. */
	int auto_release;			/* flag that says to auto-release. */
} sysvsem_sem;

extern sysvsem_module jonj_sysvsem_module;

#else

#define sysvsem_module_ptr NULL

#endif

#define jonjext_sysvsem_ptr sysvsem_module_ptr

#endif /* JONJ_SYSVSEM_H */
