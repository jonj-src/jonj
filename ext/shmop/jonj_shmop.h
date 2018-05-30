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
   | Authors: Slava Poliakov <hackie@prohost.org>                         |
   |          Ilia Alshanetsky <ilia@prohost.org>                         |
   +----------------------------------------------------------------------+
 */
#ifndef JONJ_SHMOP_H
#define JONJ_SHMOP_H

#if HAVE_SHMOP

extern zend_module_entry shmop_module_entry;
#define jonjext_shmop_ptr &shmop_module_entry

JONJ_MINIT_FUNCTION(shmop);
JONJ_MINFO_FUNCTION(shmop);

JONJ_FUNCTION(shmop_open);
JONJ_FUNCTION(shmop_read);
JONJ_FUNCTION(shmop_close);
JONJ_FUNCTION(shmop_size);
JONJ_FUNCTION(shmop_write);
JONJ_FUNCTION(shmop_delete);

#ifdef JONJ_WIN32
typedef int key_t;
#endif

struct jonj_shmop
{
	int shmid;
	key_t key;
	int shmflg;
	int shmatflg;
	char *addr;
	int size;
};

typedef struct {
	int le_shmop;
} jonj_shmop_globals;

#ifdef ZTS
#define SHMOPG(v) TSRMG(shmop_globals_id, jonj_shmop_globals *, v)
#else
#define SHMOPG(v) (shmop_globals.v)
#endif

#else

#define jonjext_shmop_ptr NULL

#endif

#endif	/* JONJ_SHMOP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
