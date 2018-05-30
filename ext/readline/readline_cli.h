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
   | Author: Marcus Boerger <helly@jonj.tk>                               |
   |         Johannes Schlueter <johannes@jonj.tk>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj.h"
#include "ext/standard/jonj_smart_str.h"

ZEND_BEGIN_MODULE_GLOBALS(cli_readline)
	char *pager;
	char *prompt;
	smart_str *prompt_str;
ZEND_END_MODULE_GLOBALS(cli_readline)

#ifdef ZTS
# define CLIR_G(v) TSRMG(cli_readline_globals_id, zend_cli_readline_globals *, v)
#else
# define CLIR_G(v) (cli_readline_globals.v)
#endif

extern JONJ_MINIT_FUNCTION(cli_readline);
extern JONJ_MSHUTDOWN_FUNCTION(cli_readline);
extern JONJ_MINFO_FUNCTION(cli_readline);

ZEND_EXTERN_MODULE_GLOBALS(cli_readline)


