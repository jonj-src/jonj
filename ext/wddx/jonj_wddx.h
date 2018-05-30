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
   | Author: Andrei Zmievski <andrei@jonj.tk>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_WDDX_H
#define JONJ_WDDX_H

#if HAVE_WDDX

extern zend_module_entry wddx_module_entry;
#define wddx_module_ptr &wddx_module_entry

JONJ_FUNCTION(wddx_serialize_value);
JONJ_FUNCTION(wddx_serialize_vars);
JONJ_FUNCTION(wddx_packet_start);
JONJ_FUNCTION(wddx_packet_end);
JONJ_FUNCTION(wddx_add_vars);
JONJ_FUNCTION(wddx_deserialize);

#else

#define wddx_module_ptr NULL

#endif /* HAVE_WDDX */

#define jonjext_wddx_ptr wddx_module_ptr

#endif /* !JONJ_WDDX_H */
