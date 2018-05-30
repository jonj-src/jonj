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
   | Authors: Rasmus Lerdorf <rasmus@jonj.tk>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef CREDITS_H
#define CREDITS_H

#ifndef HAVE_CREDITS_DEFS
#define HAVE_CREDITS_DEFS

#define JONJ_CREDITS_GROUP			(1<<0)
#define JONJ_CREDITS_GENERAL			(1<<1)
#define JONJ_CREDITS_SAPI			(1<<2)
#define JONJ_CREDITS_MODULES			(1<<3)
#define JONJ_CREDITS_DOCS			(1<<4)
#define JONJ_CREDITS_FULLPAGE		(1<<5)
#define JONJ_CREDITS_QA				(1<<6)
#define JONJ_CREDITS_WEB				(1<<7)
#define JONJ_CREDITS_ALL				0xFFFFFFFF

#endif /* HAVE_CREDITS_DEFS */

JONJAPI void jonj_print_credits(int flag TSRMLS_DC);

#endif
