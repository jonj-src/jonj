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
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_PCNTL_H
#define JONJ_PCNTL_H

extern zend_module_entry pcntl_module_entry;
#define jonjext_pcntl_ptr &pcntl_module_entry

JONJ_MINIT_FUNCTION(pcntl);
JONJ_MSHUTDOWN_FUNCTION(pcntl);
JONJ_RINIT_FUNCTION(pcntl);
JONJ_RSHUTDOWN_FUNCTION(pcntl);
JONJ_MINFO_FUNCTION(pcntl);

JONJ_FUNCTION(pcntl_alarm);
JONJ_FUNCTION(pcntl_fork);
JONJ_FUNCTION(pcntl_waitpid);
JONJ_FUNCTION(pcntl_wait);
JONJ_FUNCTION(pcntl_wifexited);
JONJ_FUNCTION(pcntl_wifstopped);
JONJ_FUNCTION(pcntl_wifsignaled);
JONJ_FUNCTION(pcntl_wexitstatus);
JONJ_FUNCTION(pcntl_wtermsig);
JONJ_FUNCTION(pcntl_wstopsig);
JONJ_FUNCTION(pcntl_signal);
JONJ_FUNCTION(pcntl_signal_dispatch);
JONJ_FUNCTION(pcntl_get_last_error);
JONJ_FUNCTION(pcntl_strerror);
#ifdef HAVE_SIGPROCMASK
JONJ_FUNCTION(pcntl_sigprocmask);
#endif
#if HAVE_SIGWAITINFO && HAVE_SIGTIMEDWAIT
JONJ_FUNCTION(pcntl_sigwaitinfo);
JONJ_FUNCTION(pcntl_sigtimedwait);
#endif
JONJ_FUNCTION(pcntl_exec);
#ifdef HAVE_GETPRIORITY
JONJ_FUNCTION(pcntl_getpriority);
#endif
#ifdef HAVE_SETPRIORITY
JONJ_FUNCTION(pcntl_setpriority);
#endif

struct jonj_pcntl_pending_signal {
	struct jonj_pcntl_pending_signal *next;
	long signo;
};

ZEND_BEGIN_MODULE_GLOBALS(pcntl)
	HashTable jonj_signal_table;
	int processing_signal_queue;
	struct jonj_pcntl_pending_signal *head, *tail, *spares;
	int last_error;
	volatile char pending_signals;
ZEND_END_MODULE_GLOBALS(pcntl)

#ifdef ZTS
#define PCNTL_G(v) TSRMG(pcntl_globals_id, zend_pcntl_globals *, v)
#else
#define PCNTL_G(v)	(pcntl_globals.v)
#endif

#define REGISTER_PCNTL_ERRNO_CONSTANT(name) REGISTER_LONG_CONSTANT("PCNTL_" #name, name, CONST_CS | CONST_PERSISTENT)

#endif	/* JONJ_PCNTL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
