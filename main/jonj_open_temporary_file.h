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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_OPEN_TEMPORARY_FILE_H
#define JONJ_OPEN_TEMPORARY_FILE_H

BEGIN_EXTERN_C()
JONJAPI FILE *jonj_open_temporary_file(const char *dir, const char *pfx, char **opened_path_p TSRMLS_DC);
JONJAPI int jonj_open_temporary_fd_ex(const char *dir, const char *pfx, char **opened_path_p, zend_bool open_basedir_check TSRMLS_DC);
JONJAPI int jonj_open_temporary_fd(const char *dir, const char *pfx, char **opened_path_p TSRMLS_DC);
JONJAPI const char *jonj_get_temporary_directory(TSRMLS_D);
JONJAPI void jonj_shutdown_temporary_directory(void);
END_EXTERN_C()

#endif /* JONJ_OPEN_TEMPORARY_FILE_H */
