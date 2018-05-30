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

#ifndef JONJ_MYSQL_H
#define JONJ_MYSQL_H

#if HAVE_MYSQL

extern zend_module_entry mysql_module_entry;
#define mysql_module_ptr &mysql_module_entry

#else
#define mysql_module_ptr NULL
#endif

#define jonjext_mysql_ptr mysql_module_ptr

#endif /* JONJ_MYSQL_H */
