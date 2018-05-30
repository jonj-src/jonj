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
   | Authors: Felipe Pena <felipe@jonj.tk>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@jonj.tk>                                |
   +----------------------------------------------------------------------+
*/

#ifndef JONJDBG_PROMPT_H
#define JONJDBG_PROMPT_H

/* {{{ */
void jonjdbg_init(char *init_file, size_t init_file_len, zend_bool use_default TSRMLS_DC);
void jonjdbg_try_file_init(char *init_file, size_t init_file_len, zend_bool free_init TSRMLS_DC);
int jonjdbg_interactive(TSRMLS_D);
int jonjdbg_compile(TSRMLS_D);
void jonjdbg_clean(zend_bool full TSRMLS_DC); /* }}} */

/* {{{ jonjdbg command handlers */
JONJDBG_COMMAND(exec);
JONJDBG_COMMAND(step);
JONJDBG_COMMAND(continue);
JONJDBG_COMMAND(run);
JONJDBG_COMMAND(ev);
JONJDBG_COMMAND(until);
JONJDBG_COMMAND(finish);
JONJDBG_COMMAND(leave);
JONJDBG_COMMAND(frame);
JONJDBG_COMMAND(print);
JONJDBG_COMMAND(break);
JONJDBG_COMMAND(back);
JONJDBG_COMMAND(list);
JONJDBG_COMMAND(info);
JONJDBG_COMMAND(clean);
JONJDBG_COMMAND(clear);
JONJDBG_COMMAND(help);
JONJDBG_COMMAND(sh);
JONJDBG_COMMAND(set);
JONJDBG_COMMAND(source);
JONJDBG_COMMAND(export);
JONJDBG_COMMAND(register);
JONJDBG_COMMAND(quit);
JONJDBG_COMMAND(watch); /* }}} */

/* {{{ prompt commands */
extern const jonjdbg_command_t jonjdbg_prompt_commands[]; /* }}} */

/* {{{ */
#if JONJ_VERSION_ID >= 50500
void jonjdbg_execute_ex(zend_execute_data *execute_data TSRMLS_DC);
#else
void jonjdbg_execute_ex(zend_op_array *op_array TSRMLS_DC);
#endif /* }}} */

#endif /* JONJDBG_PROMPT_H */
