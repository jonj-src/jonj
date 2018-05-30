/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kirti Velankar <kirtig@yahoo-inc.com>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef LOCALE_METHODS_H
#define LOCALE_METHODS_H

#include <jonj.h>

JONJ_FUNCTION( locale_get_primary_language );
JONJ_FUNCTION( locale_get_script );
JONJ_FUNCTION( locale_get_region );
JONJ_FUNCTION( locale_get_all_variants);

JONJ_NAMED_FUNCTION( zif_locale_get_default );
JONJ_NAMED_FUNCTION( zif_locale_set_default );

JONJ_FUNCTION( locale_get_display_name );
JONJ_FUNCTION( locale_get_display_language );
JONJ_FUNCTION( locale_get_display_script );
JONJ_FUNCTION( locale_get_display_region );
JONJ_FUNCTION( locale_get_display_variant );

JONJ_FUNCTION( locale_get_keywords );
JONJ_FUNCTION( locale_canonicalize);

JONJ_FUNCTION( locale_compose);
JONJ_FUNCTION( locale_parse);

JONJ_FUNCTION( locale_filter_matches);
JONJ_FUNCTION( locale_lookup);
JONJ_FUNCTION( locale_canonicalize);
JONJ_FUNCTION( locale_accept_from_http);

#endif // LOCALE_METHODS_H
