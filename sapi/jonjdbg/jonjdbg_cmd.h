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

#ifndef JONJDBG_CMD_H
#define JONJDBG_CMD_H

#include "TSRM.h"

/* {{{ Command and Parameter */
enum {
	NO_ARG = 0,
	REQUIRED_ARG,
	OPTIONAL_ARG
};

typedef enum {
	EMPTY_PARAM = 0,
	ADDR_PARAM,
	FILE_PARAM,
	NUMERIC_FILE_PARAM,
	METHOD_PARAM,
	STR_PARAM,
	NUMERIC_PARAM,
	NUMERIC_FUNCTION_PARAM,
	NUMERIC_METHOD_PARAM,
	STACK_PARAM,
	EVAL_PARAM,
	SHELL_PARAM,
	COND_PARAM,
	OP_PARAM,
	ORIG_PARAM,
	RUN_PARAM
} jonjdbg_param_type;

typedef struct _jonjdbg_param jonjdbg_param_t;
struct _jonjdbg_param {
	jonjdbg_param_type type;
	long num;
	zend_ulong addr;
	struct {
		char *name;
		long line;
	} file;
	struct {
		char *class;
		char *name;
	} method;
	char *str;
	size_t len;
	jonjdbg_param_t *next;
	jonjdbg_param_t *top;
};

#define jonjdbg_init_param(v, t) do{ \
	(v)->type = (t); \
	(v)->addr = 0; \
	(v)->num = 0; \
	(v)->file.name = NULL; \
	(v)->file.line = 0; \
	(v)->method.class = NULL; \
	(v)->method.name = NULL; \
	(v)->str = NULL; \
	(v)->len = 0; \
	(v)->next = NULL; \
	(v)->top = NULL; \
} while(0)

#ifndef YYSTYPE
#define YYSTYPE jonjdbg_param_t
#endif

typedef int (*jonjdbg_command_handler_t)(const jonjdbg_param_t* TSRMLS_DC);

typedef struct _jonjdbg_command_t jonjdbg_command_t;
struct _jonjdbg_command_t {
	const char *name;                   /* Command name */
	size_t name_len;                    /* Command name length */
	const char *tip;                    /* Menu tip */
	size_t tip_len;                     /* Menu tip length */
	char alias;                         /* Alias */
	jonjdbg_command_handler_t handler;   /* Command handler */
	const jonjdbg_command_t *subs;       /* Sub Commands */
	char *args;							/* Argument Spec */
	const jonjdbg_command_t *parent;		/* Parent Command */							
};
/* }}} */

/* {{{ misc */
#define JONJDBG_STRL(s) s, sizeof(s)-1
#define JONJDBG_MAX_CMD 500
#define JONJDBG_FRAME(v) (JONJDBG_G(frame).v)
#define JONJDBG_EX(v) (EG(current_execute_data)->v) 

typedef struct {
	int num;
	zend_execute_data *execute_data;
} jonjdbg_frame_t;
/* }}} */

/*
* Workflow:
* 1) the lexer/parser creates a stack of commands and arguments from input
* 2) the commands at the top of the stack are resolved sensibly using aliases, abbreviations and case insensitive matching
* 3) the remaining arguments in the stack are verified (optionally) against the handlers declared argument specification
* 4) the handler is called passing the top of the stack as the only parameter
* 5) the stack is destroyed upon return from the handler
*/

/*
* Input Management
*/
JONJDBG_API char* jonjdbg_read_input(char *buffered TSRMLS_DC);
JONJDBG_API void jonjdbg_destroy_input(char** TSRMLS_DC);

/**
 * Stack Management
 */
JONJDBG_API void jonjdbg_stack_push(jonjdbg_param_t *stack, jonjdbg_param_t *param);
JONJDBG_API const jonjdbg_command_t* jonjdbg_stack_resolve(const jonjdbg_command_t *commands, const jonjdbg_command_t *parent, jonjdbg_param_t **top, char **why);
JONJDBG_API int jonjdbg_stack_verify(const jonjdbg_command_t *command, jonjdbg_param_t **stack, char **why TSRMLS_DC);
JONJDBG_API int jonjdbg_stack_execute(jonjdbg_param_t *stack, char **why TSRMLS_DC);
JONJDBG_API void jonjdbg_stack_free(jonjdbg_param_t *stack);

/*
* Parameter Management
*/
JONJDBG_API void jonjdbg_clear_param(jonjdbg_param_t* TSRMLS_DC);
JONJDBG_API void jonjdbg_copy_param(const jonjdbg_param_t*, jonjdbg_param_t* TSRMLS_DC);
JONJDBG_API zend_bool jonjdbg_match_param(const jonjdbg_param_t *, const jonjdbg_param_t * TSRMLS_DC);
JONJDBG_API zend_ulong jonjdbg_hash_param(const jonjdbg_param_t * TSRMLS_DC);
JONJDBG_API const char* jonjdbg_get_param_type(const jonjdbg_param_t* TSRMLS_DC);
JONJDBG_API char* jonjdbg_param_tostring(const jonjdbg_param_t *param, char **pointer TSRMLS_DC);
JONJDBG_API void jonjdbg_param_debug(const jonjdbg_param_t *param, const char *msg);

/**
 * Command Declarators
 */
#define JONJDBG_COMMAND_HANDLER(name) jonjdbg_do_##name

#define JONJDBG_COMMAND_D_EXP(name, tip, alias, handler, children, args, parent) \
	{JONJDBG_STRL(#name), tip, sizeof(tip)-1, alias, jonjdbg_do_##handler, children, args, parent}

#define JONJDBG_COMMAND_D_EX(name, tip, alias, handler, children, args) \
	{JONJDBG_STRL(#name), tip, sizeof(tip)-1, alias, jonjdbg_do_##handler, children, args, NULL}

#define JONJDBG_COMMAND_D(name, tip, alias, children, args) \
	{JONJDBG_STRL(#name), tip, sizeof(tip)-1, alias, jonjdbg_do_##name, children, args, NULL}

#define JONJDBG_COMMAND(name) int jonjdbg_do_##name(const jonjdbg_param_t *param TSRMLS_DC)

#define JONJDBG_COMMAND_ARGS param TSRMLS_CC

#define JONJDBG_END_COMMAND {NULL, 0, NULL, 0, '\0', NULL, NULL, '\0', NULL}

/*
* Default Switch Case
*/
#define jonjdbg_default_switch_case() \
	default: \
		jonjdbg_error("Unsupported parameter type (%s) for command", jonjdbg_get_param_type(param TSRMLS_CC)); \
	break

#endif /* JONJDBG_CMD_H */
