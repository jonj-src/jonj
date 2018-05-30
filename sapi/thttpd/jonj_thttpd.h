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

#ifndef JONJ_THTTPD_H
#define JONJ_THTTPD_H

#include <sys/types.h>
#include <sys/stat.h>
#include <libhttpd.h>

void	 thttpd_jonj_shutdown(void);
void	 thttpd_jonj_init(void);
off_t	 thttpd_jonj_request(httpd_conn *hc, int show_source);

void	 thttpd_register_on_close(void (*)(int));
void	 thttpd_closed_conn(int fd);
int		 thttpd_get_fd(void);
void	 thttpd_set_dont_close(void);

#endif
