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
   | Author: Stig Bakken <ssb@jonj.tk>                                    |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_TICKS_H
#define JONJ_TICKS_H

int jonj_startup_ticks(TSRMLS_D);
void jonj_deactivate_ticks(TSRMLS_D);
void jonj_shutdown_ticks(TSRMLS_D);
void jonj_run_ticks(int count);

BEGIN_EXTERN_C()
JONJAPI void jonj_add_tick_function(void (*func)(int));
JONJAPI void jonj_remove_tick_function(void (*func)(int));
END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
