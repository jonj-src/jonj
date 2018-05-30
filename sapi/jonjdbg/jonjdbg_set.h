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

#ifndef JONJDBG_SET_H
#define JONJDBG_SET_H

#include "jonjdbg_cmd.h"

#define JONJDBG_SET(name) JONJDBG_COMMAND(set_##name)

JONJDBG_SET(prompt);
#ifndef _WIN32
JONJDBG_SET(color);
JONJDBG_SET(colors);
#endif
JONJDBG_SET(oplog);
JONJDBG_SET(break);
JONJDBG_SET(breaks);
JONJDBG_SET(quiet);
JONJDBG_SET(stepping);
JONJDBG_SET(refcount);

extern const jonjdbg_command_t jonjdbg_set_commands[];

#endif /* JONJDBG_SET_H */
