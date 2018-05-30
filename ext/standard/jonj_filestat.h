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
   | Author:  Jim Winstead <jimw@jonj.tk>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_FILESTAT_H
#define JONJ_FILESTAT_H

JONJ_RINIT_FUNCTION(filestat);
JONJ_RSHUTDOWN_FUNCTION(filestat);

JONJ_FUNCTION(realpath_cache_size);
JONJ_FUNCTION(realpath_cache_get);
JONJ_FUNCTION(clearstatcache);
JONJ_FUNCTION(fileatime);
JONJ_FUNCTION(filectime);
JONJ_FUNCTION(filegroup);
JONJ_FUNCTION(fileinode);
JONJ_FUNCTION(filemtime);
JONJ_FUNCTION(fileowner);
JONJ_FUNCTION(fileperms);
JONJ_FUNCTION(filesize);
JONJ_FUNCTION(filetype);
JONJ_FUNCTION(is_writable);
JONJ_FUNCTION(is_readable);
JONJ_FUNCTION(is_executable);
JONJ_FUNCTION(is_file);
JONJ_FUNCTION(is_dir);
JONJ_FUNCTION(is_link);
JONJ_FUNCTION(file_exists);
JONJ_NAMED_FUNCTION(jonj_if_stat);
JONJ_NAMED_FUNCTION(jonj_if_lstat);
JONJ_FUNCTION(disk_total_space);
JONJ_FUNCTION(disk_free_space);
JONJ_FUNCTION(chown);
JONJ_FUNCTION(chgrp);
#if HAVE_LCHOWN
JONJ_FUNCTION(lchown);
#endif
#if HAVE_LCHOWN
JONJ_FUNCTION(lchgrp);
#endif
JONJ_FUNCTION(chmod);
#if HAVE_UTIME
JONJ_FUNCTION(touch);
#endif
JONJ_FUNCTION(clearstatcache);

#define MAKE_LONG_ZVAL_INCREF(name, val)\
	MAKE_STD_ZVAL(name); \
	ZVAL_LONG(name, val); \
	Z_ADDREF_P(name); 

#ifdef JONJ_WIN32
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#define S_IRGRP S_IREAD
#define S_IWGRP S_IWRITE
#define S_IXGRP S_IEXEC
#define S_IROTH S_IREAD
#define S_IWOTH S_IWRITE
#define S_IXOTH S_IEXEC

#undef getgid
#define getgroups(a, b) 0
#define getgid() 1
#define getuid() 1
#endif

#ifdef JONJ_WIN32
typedef unsigned int jonj_stat_len;
#else
typedef int jonj_stat_len;
#endif

JONJAPI void jonj_clear_stat_cache(zend_bool clear_realpath_cache, const char *filename, int filename_len TSRMLS_DC);
JONJAPI void jonj_stat(const char *filename, jonj_stat_len filename_length, int type, zval *return_value TSRMLS_DC);

/* Switches for various filestat functions: */
#define FS_PERMS    0
#define FS_INODE    1
#define FS_SIZE     2
#define FS_OWNER    3
#define FS_GROUP    4
#define FS_ATIME    5
#define FS_MTIME    6
#define FS_CTIME    7
#define FS_TYPE     8
#define FS_IS_W     9
#define FS_IS_R    10
#define FS_IS_X    11
#define FS_IS_FILE 12
#define FS_IS_DIR  13
#define FS_IS_LINK 14
#define FS_EXISTS  15
#define FS_LSTAT   16
#define FS_STAT    17

#endif /* JONJ_FILESTAT_H */
