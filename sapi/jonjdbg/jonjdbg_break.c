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

#include "jonjdbg.h"
#include "jonjdbg_print.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_opcode.h"
#include "jonjdbg_break.h"
#include "jonjdbg_bp.h"
#include "jonjdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

#define JONJDBG_BREAK_COMMAND_D(f, h, a, m, l, s) \
	JONJDBG_COMMAND_D_EXP(f, h, a, m, l, s, &jonjdbg_prompt_commands[10])

/**
 * Commands
 */
const jonjdbg_command_t jonjdbg_break_commands[] = {
	JONJDBG_BREAK_COMMAND_D(at,         "specify breakpoint by location and condition",           '@', break_at,      NULL, "*c"),
	JONJDBG_BREAK_COMMAND_D(del,        "delete breakpoint by identifier number",                 '~', break_del,     NULL, "n"),
	JONJDBG_END_COMMAND
};

JONJDBG_BREAK(at) /* {{{ */
{
	jonjdbg_set_breakpoint_at(param TSRMLS_CC);

	return SUCCESS;
} /* }}} */

JONJDBG_BREAK(del) /* {{{ */
{
	jonjdbg_delete_breakpoint(param->num TSRMLS_CC);

	return SUCCESS;
} /* }}} */
