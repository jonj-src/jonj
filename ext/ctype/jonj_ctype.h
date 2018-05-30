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
   | Author: Hartmut Holzgraefe <hholzgra@jonj.tk>                        |
   +----------------------------------------------------------------------+
 */

#ifndef JONJ_CTYPE_H
#define JONJ_CTYPE_H

#if HAVE_CTYPE

extern zend_module_entry ctype_module_entry;
#define jonjext_ctype_ptr &ctype_module_entry

#else

#define jonjext_ctype_ptr NULL

#endif

#endif	/* JONJ_CTYPE_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
