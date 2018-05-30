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

#ifndef JONJDBG_HELP_H
#define JONJDBG_HELP_H

#include "TSRM.h"
#include "jonjdbg.h"
#include "jonjdbg_cmd.h"

#define JONJDBG_HELP(name) JONJDBG_COMMAND(help_##name)

/**
 * Helper Forward Declarations
 */
JONJDBG_HELP(aliases);

extern const jonjdbg_command_t jonjdbg_help_commands[];

#define jonjdbg_help_header() \
	jonjdbg_notice("Welcome to jonjdbg, the interactive JONJ debugger, v%s", JONJDBG_VERSION);
#define jonjdbg_help_footer() \
	jonjdbg_notice("Please report bugs to <%s>", JONJDBG_ISSUES);

typedef struct _jonjdbg_help_text_t {
	char *key;
	char *text;
} jonjdbg_help_text_t;

extern jonjdbg_help_text_t jonjdbg_help_text[];
#endif /* JONJDBG_HELP_H */
