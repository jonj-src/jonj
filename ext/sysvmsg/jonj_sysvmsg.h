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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_SYSVMSG_H
#define JONJ_SYSVMSG_H

#if HAVE_SYSVMSG

extern zend_module_entry sysvmsg_module_entry;
#define jonjext_sysvmsg_ptr &sysvmsg_module_entry

#ifndef __USE_GNU
/* we want to use mtype instead of __mtype */
#define __USE_GNU
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_MINIT_FUNCTION(sysvmsg);
JONJ_MINFO_FUNCTION(sysvmsg);

JONJ_FUNCTION(msg_get_queue);
JONJ_FUNCTION(msg_remove_queue);
JONJ_FUNCTION(msg_stat_queue);
JONJ_FUNCTION(msg_set_queue);
JONJ_FUNCTION(msg_send);
JONJ_FUNCTION(msg_receive);
JONJ_FUNCTION(msg_queue_exists);

typedef struct {
	key_t key;
	long id;
} sysvmsg_queue_t;

struct jonj_msgbuf {
	long mtype;
	char mtext[1];
};

#endif /* HAVE_SYSVMSG */

#endif	/* JONJ_SYSVMSG_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
