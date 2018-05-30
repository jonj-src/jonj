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
   | Author: Christian Cartus <cartus@atrior.de>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SYSVSHM_H
#define JONJ_SYSVSHM_H

#if HAVE_SYSVSHM

extern zend_module_entry sysvshm_module_entry;
#define sysvshm_module_ptr &sysvshm_module_entry

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define JONJ_SHM_RSRC_NAME "sysvshm"

typedef struct {
	int le_shm;
	long init_mem;
} sysvshm_module;

typedef struct {
	long key;
	long length;
	long next;
	char mem;
} sysvshm_chunk;

typedef struct {
	char magic[8];
	long start;
	long end;
	long free;
	long total;
} sysvshm_chunk_head;

typedef struct {
	key_t key;               /* key set by user */
	long id;                 /* returned by shmget */
	sysvshm_chunk_head *ptr; /* memory address of shared memory */
} sysvshm_shm;

JONJ_MINIT_FUNCTION(sysvshm);
JONJ_FUNCTION(shm_attach);
JONJ_FUNCTION(shm_detach);
JONJ_FUNCTION(shm_remove);
JONJ_FUNCTION(shm_put_var);
JONJ_FUNCTION(shm_get_var);
JONJ_FUNCTION(shm_has_var);
JONJ_FUNCTION(shm_remove_var);

extern sysvshm_module jonj_sysvshm;

#else

#define sysvshm_module_ptr NULL

#endif

#define jonjext_sysvshm_ptr sysvshm_module_ptr

#endif /* JONJ_SYSVSHM_H */