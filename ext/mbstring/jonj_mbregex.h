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
   | Author: Moriyoshi Koizumi <moriyoshi@jonj.tk>                        |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
 
#ifndef _JONJ_MBREGEX_H
#define _JONJ_MBREGEX_H

#if HAVE_MBREGEX

#include "jonj.h"
#include "zend.h"

/* {{{ JONJ_MBREGEX_FUNCTION_ENTRIES */
#define JONJ_MBREGEX_FUNCTION_ENTRIES \
	JONJ_FE(mb_regex_encoding,	arginfo_mb_regex_encoding) \
	JONJ_FE(mb_regex_set_options,	arginfo_mb_regex_set_options) \
	JONJ_FE(mb_ereg,			arginfo_mb_ereg) \
	JONJ_FE(mb_eregi,			arginfo_mb_eregi) \
	JONJ_FE(mb_ereg_replace,			arginfo_mb_ereg_replace) \
	JONJ_FE(mb_eregi_replace,			arginfo_mb_eregi_replace) \
	JONJ_FE(mb_ereg_replace_callback,			arginfo_mb_ereg_replace_callback) \
	JONJ_FE(mb_split,					arginfo_mb_split) \
	JONJ_FE(mb_ereg_match,			arginfo_mb_ereg_match) \
	JONJ_FE(mb_ereg_search,			arginfo_mb_ereg_search) \
	JONJ_FE(mb_ereg_search_pos,		arginfo_mb_ereg_search_pos) \
	JONJ_FE(mb_ereg_search_regs,		arginfo_mb_ereg_search_regs) \
	JONJ_FE(mb_ereg_search_init,		arginfo_mb_ereg_search_init) \
	JONJ_FE(mb_ereg_search_getregs,	arginfo_mb_ereg_search_getregs) \
	JONJ_FE(mb_ereg_search_getpos,	arginfo_mb_ereg_search_getpos) \
	JONJ_FE(mb_ereg_search_setpos,	arginfo_mb_ereg_search_setpos) \
	JONJ_FALIAS(mbregex_encoding,	mb_regex_encoding,	arginfo_mb_regex_encoding) \
	JONJ_FALIAS(mbereg,	mb_ereg,	arginfo_mb_ereg) \
	JONJ_FALIAS(mberegi,	mb_eregi,	arginfo_mb_eregi) \
	JONJ_FALIAS(mbereg_replace,	mb_ereg_replace,	arginfo_mb_ereg_replace) \
	JONJ_FALIAS(mberegi_replace,	mb_eregi_replace,	arginfo_mb_eregi_replace) \
	JONJ_FALIAS(mbsplit,	mb_split,	arginfo_mb_split) \
	JONJ_FALIAS(mbereg_match,	mb_ereg_match,	arginfo_mb_ereg_match) \
	JONJ_FALIAS(mbereg_search,	mb_ereg_search,	arginfo_mb_ereg_search) \
	JONJ_FALIAS(mbereg_search_pos,	mb_ereg_search_pos,	arginfo_mb_ereg_search_pos) \
	JONJ_FALIAS(mbereg_search_regs,	mb_ereg_search_regs,	arginfo_mb_ereg_search_regs) \
	JONJ_FALIAS(mbereg_search_init,	mb_ereg_search_init,	arginfo_mb_ereg_search_init) \
	JONJ_FALIAS(mbereg_search_getregs,	mb_ereg_search_getregs,	arginfo_mb_ereg_search_getregs) \
	JONJ_FALIAS(mbereg_search_getpos,	mb_ereg_search_getpos,	arginfo_mb_ereg_search_getpos) \
	JONJ_FALIAS(mbereg_search_setpos,	mb_ereg_search_setpos,	arginfo_mb_ereg_search_setpos)
/* }}} */

#define JONJ_MBREGEX_MAXCACHE 50

JONJ_MINIT_FUNCTION(mb_regex);
JONJ_MSHUTDOWN_FUNCTION(mb_regex);
JONJ_RINIT_FUNCTION(mb_regex);
JONJ_RSHUTDOWN_FUNCTION(mb_regex);
JONJ_MINFO_FUNCTION(mb_regex);

typedef struct _zend_mb_regex_globals zend_mb_regex_globals;

zend_mb_regex_globals *jonj_mb_regex_globals_alloc(TSRMLS_D);
void jonj_mb_regex_globals_free(zend_mb_regex_globals *pglobals TSRMLS_DC);
int jonj_mb_regex_set_mbctype(const char *enc TSRMLS_DC);
int jonj_mb_regex_set_default_mbctype(const char *encname TSRMLS_DC);
const char *jonj_mb_regex_get_mbctype(TSRMLS_D);
const char *jonj_mb_regex_get_default_mbctype(TSRMLS_D);

JONJ_FUNCTION(mb_regex_encoding);
JONJ_FUNCTION(mb_ereg);
JONJ_FUNCTION(mb_eregi);
JONJ_FUNCTION(mb_ereg_replace);
JONJ_FUNCTION(mb_eregi_replace);
JONJ_FUNCTION(mb_ereg_replace_callback);
JONJ_FUNCTION(mb_split);
JONJ_FUNCTION(mb_ereg_match);
JONJ_FUNCTION(mb_ereg_search);
JONJ_FUNCTION(mb_ereg_search_pos);
JONJ_FUNCTION(mb_ereg_search_regs);
JONJ_FUNCTION(mb_ereg_search_init);
JONJ_FUNCTION(mb_ereg_search_getregs);
JONJ_FUNCTION(mb_ereg_search_getpos);
JONJ_FUNCTION(mb_ereg_search_setpos);
JONJ_FUNCTION(mb_regex_set_options);

#endif /* HAVE_MBREGEX */

#endif /* _JONJ_MBREGEX_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

