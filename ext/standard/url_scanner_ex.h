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
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef URL_SCANNER_EX_H
#define URL_SCANNER_EX_H

JONJ_MINIT_FUNCTION(url_scanner_ex);
JONJ_MSHUTDOWN_FUNCTION(url_scanner_ex);

JONJ_RINIT_FUNCTION(url_scanner_ex);
JONJ_RSHUTDOWN_FUNCTION(url_scanner_ex);

JONJAPI char *jonj_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen TSRMLS_DC);
JONJAPI int jonj_url_scanner_add_var(char *name, int name_len, char *value, int value_len, int urlencode TSRMLS_DC);
JONJAPI int jonj_url_scanner_reset_vars(TSRMLS_D);

#include "jonj_smart_str_public.h"

typedef struct {
	/* Used by the mainloop of the scanner */
	smart_str tag; /* read only */
	smart_str arg; /* read only */
	smart_str val; /* read only */
	smart_str buf;

	/* The result buffer */
	smart_str result;

	/* The data which is appended to each relative URL/FORM */
	smart_str form_app, url_app;

	int active;

	char *lookup_data;
	int state;
	
	/* Everything above is zeroed in RINIT */
	HashTable *tags;
} url_adapt_state_ex_t;

#endif
