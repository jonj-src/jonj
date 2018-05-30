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
   | If you did not receive a copy of the JONJ license and are unable to	  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kristian Koehntopp <kris@koehntopp.de>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_RECODE_H
#define JONJ_RECODE_H

#if HAVE_LIBRECODE

extern zend_module_entry recode_module_entry;
#define jonjext_recode_ptr &recode_module_entry

JONJ_MINIT_FUNCTION(recode);
JONJ_MSHUTDOWN_FUNCTION(recode);
JONJ_MINFO_FUNCTION(recode);
JONJ_FUNCTION(recode_string);
JONJ_FUNCTION(recode_file);

#else
#define jonjext_recode_ptr NULL
#endif
	
#endif /* JONJ_RECODE_H */
