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

#ifndef JONJDBG_LEXER_H
#define JONJDBG_LEXER_H

#include "jonjdbg_cmd.h"

typedef struct {
        unsigned int len;
        unsigned char *text;
        unsigned char *cursor;
        unsigned char *marker;
        int state;
} jonjdbg_lexer_data;

#define yyparse jonjdbg_parse
#define yylex jonjdbg_lex

void jonjdbg_init_lexer (jonjdbg_param_t *stack, char *input TSRMLS_DC);

int jonjdbg_lex (jonjdbg_param_t* yylval);

#endif
