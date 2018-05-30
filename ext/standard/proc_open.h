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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef JONJ_WIN32
typedef HANDLE jonj_file_descriptor_t;
typedef DWORD jonj_process_id_t;
#else
typedef int jonj_file_descriptor_t;
typedef pid_t jonj_process_id_t;
#endif

#define JONJ_PROC_OPEN_MAX_DESCRIPTORS	16

/* Environment block under win32 is a NUL terminated sequence of NUL terminated
 * name=value strings.
 * Under unix, it is an argv style array.
 * */
typedef struct _jonj_process_env {
	char *envp;
#ifndef JONJ_WIN32
	char **envarray;
#endif
} jonj_process_env_t;

struct jonj_process_handle {
	jonj_process_id_t	child;
#ifdef JONJ_WIN32
	HANDLE childHandle;
#endif
	int npipes;
	long pipes[JONJ_PROC_OPEN_MAX_DESCRIPTORS];
	char *command;
	int is_persistent;
	jonj_process_env_t env;
};

