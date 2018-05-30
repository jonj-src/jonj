%{
 
/*
 * jonjdbg_parser.y
 * (from jonj-src root)
 * flex sapi/jonjdbg/dev/jonjdbg_lexer.l
 * bison sapi/jonjdbg/dev/jonjdbg_parser.y
 */
 
#include "jonjdbg.h"
#include "jonjdbg_cmd.h"
#include "jonjdbg_utils.h"
#include "jonjdbg_cmd.h"
#include "jonjdbg_prompt.h"

#define YYSTYPE jonjdbg_param_t

#include "jonjdbg_parser.h"
#include "jonjdbg_lexer.h"

#undef yyerror
static int yyerror(void ***tsrm_ls, const char *msg);

ZEND_EXTERN_MODULE_GLOBALS(jonjdbg);

%}

%pure-parser
%error-verbose
 
%code requires {
#include "jonjdbg.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}

%parse-param { void *tsrm_ls }

%output  "sapi/jonjdbg/jonjdbg_parser.c"
%defines "sapi/jonjdbg/jonjdbg_parser.h"

%token T_EVAL       "eval"
%token T_RUN        "run"
%token T_SHELL      "shell"
%token T_IF         "if (condition)"
%token T_TRUTHY     "truthy (true, on, yes or enabled)"
%token T_FALSY      "falsy (false, off, no or disabled)"
%token T_STRING     "string (some input, perhaps)"
%token T_COLON      ": (colon)"
%token T_DCOLON     ":: (double colon)"
%token T_POUND      "# (pound sign)"
%token T_PROTO      "protocol (file://)"
%token T_DIGITS     "digits (numbers)"
%token T_LITERAL    "literal (string)"
%token T_ADDR       "address"
%token T_OPCODE     "opcode"
%token T_ID         "identifier (command or function name)"
%token T_INPUT      "input (input string or data)"
%token T_UNEXPECTED "input"

%% /* Rules */

input
	: parameters
	| full_expression { jonjdbg_stack_push(JONJDBG_G(parser_stack), &$1); }
	| /* nothing */
	;

parameters
	: parameter { jonjdbg_stack_push(JONJDBG_G(parser_stack), &$1); }
	| parameters parameter { jonjdbg_stack_push(JONJDBG_G(parser_stack), &$2); }
	;

parameter
	: T_ID T_COLON T_DIGITS { 	
		$$.type = FILE_PARAM;
		$$.file.name = $2.str;
		$$.file.line = $3.num;
	}
	| T_ID T_COLON T_POUND T_DIGITS {
		$$.type = NUMERIC_FILE_PARAM;
		$$.file.name = $1.str;
		$$.file.line = $4.num;
	}
	| T_PROTO T_ID T_COLON T_DIGITS {
		$$.type = FILE_PARAM;
		$$.file.name = malloc($1.len + $2.len + 1);
		if ($$.file.name) {
			memcpy(&$$.file.name[0], $1.str, $1.len);
			memcpy(&$$.file.name[$1.len], $2.str, $2.len);
			$$.file.name[$1.len + $2.len] = '\0';
		}
		$$.file.line = $4.num;
	}
	| T_PROTO T_ID T_COLON T_POUND T_DIGITS {
		$$.type = NUMERIC_FILE_PARAM;
		$$.file.name = malloc($1.len + $2.len + 1);
		if ($$.file.name) {
			memcpy(&$$.file.name[0], $1.str, $1.len);
			memcpy(&$$.file.name[$1.len], $2.str, $2.len);
			$$.file.name[$1.len + $2.len] = '\0';
		}
		$$.file.line = $5.num;
	}
	| T_ID T_DCOLON T_ID { 
		$$.type = METHOD_PARAM;
		$$.method.class = $1.str;
		$$.method.name = $3.str;
	}
	| T_ID T_DCOLON T_ID T_POUND T_DIGITS { 
		$$.type = NUMERIC_METHOD_PARAM;
		$$.method.class = $1.str;
		$$.method.name = $3.str;
		$$.num = $5.num; 
	}
	| T_ID T_POUND T_DIGITS {
		$$.type = NUMERIC_FUNCTION_PARAM;
		$$.str = $1.str;
		$$.len = $1.len;
		$$.num = $3.num; 
	}
	| T_IF T_INPUT {
		$$.type = COND_PARAM; 
		$$.str = $2.str;
		$$.len = $2.len;
	}
	| T_OPCODE { $$ = $1; }
	| T_ADDR { $$ = $1; }
	| T_LITERAL { $$ = $1; }
	| T_TRUTHY { $$ = $1; }
	| T_FALSY { $$ = $1; }
	| T_DIGITS { $$ = $1; }
	| T_ID { $$ = $1; }
	;

full_expression
	: T_EVAL T_INPUT { 
		$$.type = EVAL_PARAM; 
		$$.str = $2.str;
		$$.len = $2.len;
	}
	| T_SHELL T_INPUT { 	
		$$.type = SHELL_PARAM; 
		$$.str = $2.str;
		$$.len = $2.len;
	}
	| T_RUN {
		$$.type = RUN_PARAM;
		$$.len = 0;
	}
	| T_RUN T_INPUT { 	
		$$.type = RUN_PARAM; 
		$$.str = $2.str;
		$$.len = $2.len;
	}
	;

%%

static int yyerror(void ***tsrm_ls, const char *msg) {
	jonjdbg_error("Parse Error: %s", msg);

	{
		const jonjdbg_param_t *top = JONJDBG_G(parser_stack);
    	
		while (top) {
			jonjdbg_param_debug(top, "--> ");
			top = top->next;
		}
	}
	return 0;
}

int jonjdbg_do_parse(jonjdbg_param_t *stack, char *input TSRMLS_DC) {
	jonjdbg_init_lexer(stack, input TSRMLS_CC);

#ifdef ZTS
	return yyparse(TSRMLS_C);
#else
	return yyparse(NULL);
#endif
}
