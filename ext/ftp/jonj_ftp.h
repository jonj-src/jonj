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
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@jonj.tk> (resume functions)            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef	_INCLUDED_FTP_H
#define	_INCLUDED_FTP_H

#if HAVE_FTP

extern zend_module_entry jonj_ftp_module_entry;
#define jonj_ftp_module_ptr &jonj_ftp_module_entry

#define JONJ_FTP_OPT_TIMEOUT_SEC	0
#define JONJ_FTP_OPT_AUTOSEEK	1
#define JONJ_FTP_OPT_USEPASVADDRESS	2
#define JONJ_FTP_AUTORESUME		-1

JONJ_MINIT_FUNCTION(ftp);
JONJ_MINFO_FUNCTION(ftp);

JONJ_FUNCTION(ftp_connect);
#ifdef HAVE_OPENSSL_EXT
JONJ_FUNCTION(ftp_ssl_connect);
#endif
JONJ_FUNCTION(ftp_login);
JONJ_FUNCTION(ftp_pwd);
JONJ_FUNCTION(ftp_cdup);
JONJ_FUNCTION(ftp_chdir);
JONJ_FUNCTION(ftp_exec);
JONJ_FUNCTION(ftp_raw);
JONJ_FUNCTION(ftp_mkdir);
JONJ_FUNCTION(ftp_rmdir);
JONJ_FUNCTION(ftp_chmod);
JONJ_FUNCTION(ftp_alloc);
JONJ_FUNCTION(ftp_nlist);
JONJ_FUNCTION(ftp_rawlist);
JONJ_FUNCTION(ftp_systype);
JONJ_FUNCTION(ftp_pasv);
JONJ_FUNCTION(ftp_get);
JONJ_FUNCTION(ftp_fget);
JONJ_FUNCTION(ftp_put);
JONJ_FUNCTION(ftp_fput);
JONJ_FUNCTION(ftp_size);
JONJ_FUNCTION(ftp_mdtm);
JONJ_FUNCTION(ftp_rename);
JONJ_FUNCTION(ftp_delete);
JONJ_FUNCTION(ftp_site);
JONJ_FUNCTION(ftp_close);
JONJ_FUNCTION(ftp_set_option);
JONJ_FUNCTION(ftp_get_option);
JONJ_FUNCTION(ftp_nb_get);
JONJ_FUNCTION(ftp_nb_fget);
JONJ_FUNCTION(ftp_nb_put);
JONJ_FUNCTION(ftp_nb_fput);
JONJ_FUNCTION(ftp_nb_continue);

#define jonjext_ftp_ptr jonj_ftp_module_ptr

#else
#define jonj_ftp_module_ptr NULL
#endif	/* HAVE_FTP */

#endif
