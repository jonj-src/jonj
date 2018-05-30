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
   | Author: Jim Winstead <jimw@jonj.tk>                                  |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef FOPEN_WRAPPERS_H
#define FOPEN_WRAPPERS_H

BEGIN_EXTERN_C()
#include "jonj_globals.h"
#include "jonj_ini.h"

JONJAPI int jonj_fopen_primary_script(zend_file_handle *file_handle TSRMLS_DC);
JONJAPI char *expand_filepath(const char *filepath, char *real_path TSRMLS_DC);
JONJAPI char *expand_filepath_ex(const char *filepath, char *real_path, const char *relative_to, size_t relative_to_len TSRMLS_DC);
JONJAPI char *expand_filepath_with_mode(const char *filepath, char *real_path, const char *relative_to, size_t relative_to_len, int use_realpath TSRMLS_DC);

JONJAPI int jonj_check_open_basedir(const char *path TSRMLS_DC);
JONJAPI int jonj_check_open_basedir_ex(const char *path, int warn TSRMLS_DC);
JONJAPI int jonj_check_specific_open_basedir(const char *basedir, const char *path TSRMLS_DC);

/* {{{ OPENBASEDIR_CHECKPATH(filename) to ease merge between 6.x and 5.x */
#if JONJ_API_VERSION < 20100412
# define OPENBASEDIR_CHECKPATH(filename) \
	(PG(safe_mode) && (!jonj_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || jonj_check_open_basedir(filename TSRMLS_CC)
#else
#define OPENBASEDIR_CHECKPATH(filename) \
	jonj_check_open_basedir(filename TSRMLS_CC)
#endif
/* }}} */

JONJAPI int jonj_check_safe_mode_include_dir(const char *path TSRMLS_DC);

JONJAPI char *jonj_resolve_path(const char *filename, int filename_len, const char *path TSRMLS_DC);

JONJAPI FILE *jonj_fopen_with_path(const char *filename, const char *mode, const char *path, char **opened_path TSRMLS_DC);

JONJAPI char *jonj_strip_url_passwd(char *path);

JONJAPI ZEND_INI_MH(OnUpdateBaseDir);
END_EXTERN_C()

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
