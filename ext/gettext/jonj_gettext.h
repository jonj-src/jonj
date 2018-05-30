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
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_GETTEXT_H
#define JONJ_GETTEXT_H

#if HAVE_LIBINTL

extern zend_module_entry jonj_gettext_module_entry;
#define gettext_module_ptr &jonj_gettext_module_entry

JONJ_MINFO_FUNCTION(jonj_gettext);

JONJ_NAMED_FUNCTION(zif_textdomain);
JONJ_NAMED_FUNCTION(zif_gettext);
JONJ_NAMED_FUNCTION(zif_dgettext);
JONJ_NAMED_FUNCTION(zif_dcgettext);
JONJ_NAMED_FUNCTION(zif_bindtextdomain);
#if HAVE_NGETTEXT
JONJ_NAMED_FUNCTION(zif_ngettext);
#endif
#if HAVE_DNGETTEXT
JONJ_NAMED_FUNCTION(zif_dngettext);
#endif
#if HAVE_DCNGETTEXT
JONJ_NAMED_FUNCTION(zif_dcngettext);
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
JONJ_NAMED_FUNCTION(zif_bind_textdomain_codeset);
#endif

#else
#define gettext_module_ptr NULL
#endif /* HAVE_LIBINTL */

#define jonjext_gettext_ptr gettext_module_ptr

#endif /* JONJ_GETTEXT_H */
