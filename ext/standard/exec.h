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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef EXEC_H
#define EXEC_H

JONJ_FUNCTION(system);
JONJ_FUNCTION(exec);
JONJ_FUNCTION(escapeshellcmd);
JONJ_FUNCTION(escapeshellarg);
JONJ_FUNCTION(passthru);
JONJ_FUNCTION(shell_exec);
JONJ_FUNCTION(proc_open);
JONJ_FUNCTION(proc_get_status);
JONJ_FUNCTION(proc_close);
JONJ_FUNCTION(proc_terminate);
JONJ_FUNCTION(proc_nice);
JONJ_MINIT_FUNCTION(proc_open);
JONJ_MINIT_FUNCTION(exec);

JONJAPI char *jonj_escape_shell_cmd(char *);
JONJAPI char *jonj_escape_shell_arg(char *);
JONJAPI int jonj_exec(int type, char *cmd, zval *array, zval *return_value TSRMLS_DC);

#endif /* EXEC_H */
