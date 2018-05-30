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
#include "jonjdbg_cmd.h"
#include "jonjdbg_set.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_bp.h"
#include "jonjdbg_prompt.h"

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

#define JONJDBG_SET_COMMAND_D(f, h, a, m, l, s) \
	JONJDBG_COMMAND_D_EXP(f, h, a, m, l, s, &jonjdbg_prompt_commands[18])

const jonjdbg_command_t jonjdbg_set_commands[] = {
	JONJDBG_SET_COMMAND_D(prompt,       "usage: set prompt [<string>]",            'p', set_prompt,       NULL, "|s"),
#ifndef _WIN32
	JONJDBG_SET_COMMAND_D(color,        "usage: set color  <element> <color>",     'c', set_color,        NULL, "ss"),
	JONJDBG_SET_COMMAND_D(colors,       "usage: set colors [<on|off>]",            'C', set_colors,       NULL, "|b"),
#endif
	JONJDBG_SET_COMMAND_D(oplog,        "usage: set oplog  [<output>]",            'O', set_oplog,        NULL, "|s"),
	JONJDBG_SET_COMMAND_D(break,        "usage: set break id [<on|off>]",          'b', set_break,        NULL, "l|b"),
	JONJDBG_SET_COMMAND_D(breaks,       "usage: set breaks [<on|off>]",            'B', set_breaks,       NULL, "|b"),
	JONJDBG_SET_COMMAND_D(quiet,        "usage: set quiet [<on|off>]",             'q', set_quiet,        NULL, "|b"),
	JONJDBG_SET_COMMAND_D(stepping,     "usage: set stepping [<line|op>]",         's', set_stepping,     NULL, "|s"),
	JONJDBG_SET_COMMAND_D(refcount,     "usage: set refcount [<on|off>]",          'r', set_refcount,     NULL, "|b"),
	JONJDBG_END_COMMAND
};

JONJDBG_SET(prompt) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_writeln("%s", jonjdbg_get_prompt(TSRMLS_C));
	} else jonjdbg_set_prompt(param->str TSRMLS_CC);
	
	return SUCCESS;
} /* }}} */

JONJDBG_SET(break) /* {{{ */
{
	switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->next) {
				if (param->next->num) {
					jonjdbg_enable_breakpoint(param->num TSRMLS_CC);
				} else jonjdbg_disable_breakpoint(param->num TSRMLS_CC);
			} else {
				jonjdbg_breakbase_t *brake = jonjdbg_find_breakbase(param->num TSRMLS_CC);
				if (brake) {
					jonjdbg_writeln(
						"%s", brake->disabled ? "off" : "on");
				} else {
					jonjdbg_error("Failed to find breakpoint #%ld", param->num);
				}
			}
		} break;

		default:
			jonjdbg_error(
				"set break used incorrectly: set break [id] <on|off>");
	}

	return SUCCESS;
} /* }}} */

JONJDBG_SET(breaks) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_writeln("%s",
			JONJDBG_G(flags) & JONJDBG_IS_BP_ENABLED ? "on" : "off");
	} else switch (param->type) {	
		case NUMERIC_PARAM: {
			if (param->num) {
				jonjdbg_enable_breakpoints(TSRMLS_C);
			} else jonjdbg_disable_breakpoints(TSRMLS_C);
		} break;

		default:
			jonjdbg_error(
				"set break used incorrectly: set break [id] <on|off>");
	}

	return SUCCESS;
} /* }}} */

#ifndef _WIN32
JONJDBG_SET(color) /* {{{ */
{
	const jonjdbg_color_t *color = jonjdbg_get_color(
			param->next->str, param->next->len TSRMLS_CC);
	
	if (!color) {
		jonjdbg_error(
			"Failed to find the requested color (%s)", param->next->str);
		return SUCCESS;
	}
	
	switch (jonjdbg_get_element(param->str, param->len TSRMLS_CC)) {
		case JONJDBG_COLOR_PROMPT:
			jonjdbg_notice(
				"setting prompt color to %s (%s)", color->name, color->code);
			if (JONJDBG_G(prompt)[1]) {
				free(JONJDBG_G(prompt)[1]);
				JONJDBG_G(prompt)[1]=NULL;
			}
			jonjdbg_set_color(JONJDBG_COLOR_PROMPT, color TSRMLS_CC);
		break;
		
		case JONJDBG_COLOR_ERROR:
			jonjdbg_notice(
				"setting error color to %s (%s)", color->name, color->code);
			jonjdbg_set_color(JONJDBG_COLOR_ERROR, color TSRMLS_CC);
		break;
		
		case JONJDBG_COLOR_NOTICE:
			jonjdbg_notice(
				"setting notice color to %s (%s)", color->name, color->code);
			jonjdbg_set_color(JONJDBG_COLOR_NOTICE, color TSRMLS_CC);
		break;
		
		default:
			jonjdbg_error(
				"Failed to find the requested element (%s)", param->str);
	}

	return SUCCESS;
} /* }}} */

JONJDBG_SET(colors) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_writeln("%s", JONJDBG_G(flags) & JONJDBG_IS_COLOURED ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				JONJDBG_G(flags) |= JONJDBG_IS_COLOURED;
			} else {
				JONJDBG_G(flags) &= ~JONJDBG_IS_COLOURED;
			}
		} break;
		
		default:
			jonjdbg_error(
				"set colors used incorrectly: set colors <on|off>");
	}

	return SUCCESS;
} /* }}} */
#endif

JONJDBG_SET(oplog) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_notice("Oplog %s", JONJDBG_G(oplog) ? "enabled" : "disabled");
	} else switch (param->type) {
		case STR_PARAM: {
			/* open oplog */
			FILE *old = JONJDBG_G(oplog);

			JONJDBG_G(oplog) = fopen(param->str, "w+");
			if (!JONJDBG_G(oplog)) {
				jonjdbg_error("Failed to open %s for oplog", param->str);
				JONJDBG_G(oplog) = old;
			} else {
				if (old) {
					jonjdbg_notice("Closing previously open oplog");
					fclose(old);
				}
				jonjdbg_notice("Successfully opened oplog %s", param->str);
			}
		} break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

JONJDBG_SET(quiet) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_writeln("Quietness %s", 
			JONJDBG_G(flags) & JONJDBG_IS_QUIET ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				JONJDBG_G(flags) |= JONJDBG_IS_QUIET;
			} else {
				JONJDBG_G(flags) &= ~JONJDBG_IS_QUIET;
			}
		} break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

JONJDBG_SET(stepping) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_writeln("Stepping %s",
			JONJDBG_G(flags) & JONJDBG_STEP_OPCODE ? "opcode" : "line");
	} else switch (param->type) {
		case STR_PARAM: {	
			if ((param->len == sizeof("opcode")-1) &&
				(memcmp(param->str, "opcode", sizeof("opcode")) == SUCCESS)) {
				JONJDBG_G(flags) |= JONJDBG_STEP_OPCODE;
			} else if ((param->len == sizeof("line")-1) &&
				(memcmp(param->str, "line", sizeof("line")) == SUCCESS)) {
				JONJDBG_G(flags) &= ~JONJDBG_STEP_OPCODE;
			} else {
				jonjdbg_error("usage set stepping [<opcode|line>]");
			}
		} break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

JONJDBG_SET(refcount) /* {{{ */
{
	if (!param || param->type == EMPTY_PARAM) {
		jonjdbg_writeln("Refcount %s", JONJDBG_G(flags) & JONJDBG_IS_QUIET ? "on" : "off");
	} else switch (param->type) {
		case NUMERIC_PARAM: {
			if (param->num) {
				JONJDBG_G(flags) |= JONJDBG_SHOW_REFCOUNTS;
			} else {
				JONJDBG_G(flags) &= ~JONJDBG_SHOW_REFCOUNTS;
			}
		} break;

		jonjdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */
