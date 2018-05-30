/*
  +----------------------------------------------------------------------+
  | JONJ Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The JONJ Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the JONJ license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.jonj.tk/license/3_0.txt.                                  |
  | If you did not receive a copy of the JONJ license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@jonj.tk so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ilia Alshanetsky <ilia@jonj.tk>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_FILEINFO_H
#define JONJ_FILEINFO_H

extern zend_module_entry fileinfo_module_entry;
#define jonjext_fileinfo_ptr &fileinfo_module_entry

#define JONJ_FILEINFO_VERSION "1.0.5"

#ifdef JONJ_WIN32
#define JONJ_FILEINFO_API __declspec(dllexport)
#else
#define JONJ_FILEINFO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

JONJ_MINFO_FUNCTION(fileinfo);

JONJ_FUNCTION(finfo_open);
JONJ_FUNCTION(finfo_close);
JONJ_FUNCTION(finfo_set_flags);
JONJ_FUNCTION(finfo_file);
JONJ_FUNCTION(finfo_buffer);
JONJ_FUNCTION(mime_content_type);

#ifdef ZTS
#define FILEINFO_G(v) TSRMG(fileinfo_globals_id, zend_fileinfo_globals *, v)
#else
#define FILEINFO_G(v) (fileinfo_globals.v)
#endif

#endif	/* JONJ_FILEINFO_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
