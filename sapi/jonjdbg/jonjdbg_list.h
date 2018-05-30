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

#ifndef JONJDBG_LIST_H
#define JONJDBG_LIST_H

#include "TSRM.h"
#include "jonjdbg_cmd.h"

#define JONJDBG_LIST(name)         JONJDBG_COMMAND(list_##name)
#define JONJDBG_LIST_HANDLER(name) JONJDBG_COMMAND_HANDLER(list_##name)

JONJDBG_LIST(lines);
JONJDBG_LIST(class);
JONJDBG_LIST(method);
JONJDBG_LIST(func);

void jonjdbg_list_function_byname(const char *, size_t TSRMLS_DC);
void jonjdbg_list_function(const zend_function* TSRMLS_DC);
void jonjdbg_list_file(const char*, long, long, int TSRMLS_DC);

extern const jonjdbg_command_t jonjdbg_list_commands[];

#endif /* JONJDBG_LIST_H */
