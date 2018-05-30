/*
  +----------------------------------------------------------------------+
  | phar jonj single-file executable JONJ extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the JONJ license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_01.txt.                                 |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@jonj.tk>                             |
  |          Marcus Boerger <helly@jonj.tk>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_PHAR_H
#define JONJ_PHAR_H

#define JONJ_PHAR_VERSION "2.0.2"

#include "ext/standard/basic_functions.h"
extern zend_module_entry phar_module_entry;
#define jonjext_phar_ptr &phar_module_entry

#ifdef JONJ_WIN32
#define JONJ_PHAR_API __declspec(dllexport)
#else
#define JONJ_PHAR_API JONJAPI
#endif

JONJ_PHAR_API int phar_resolve_alias(char *alias, int alias_len, char **filename, int *filename_len TSRMLS_DC);

#endif /* JONJ_PHAR_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
