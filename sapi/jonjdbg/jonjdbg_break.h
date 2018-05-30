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

#ifndef JONJDBG_BREAK_H
#define JONJDBG_BREAK_H

#include "TSRM.h"
#include "jonjdbg_cmd.h"

#define JONJDBG_BREAK(name) JONJDBG_COMMAND(break_##name)

/**
 * Printer Forward Declarations
 */
JONJDBG_BREAK(at);
JONJDBG_BREAK(del);

extern const jonjdbg_command_t jonjdbg_break_commands[];

#endif /* JONJDBG_BREAK_H */
