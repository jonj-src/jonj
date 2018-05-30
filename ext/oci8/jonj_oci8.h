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
   | Authors: Stig Sæther Bakken <ssb@jonj.tk>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#if HAVE_OCI8
# ifndef JONJ_OCI8_H
#  define JONJ_OCI8_H

#ifdef ZTS
# include "TSRM.h"
#endif


/*
 * The version of the OCI8 extension.
 */
#ifdef JONJ_OCI8_VERSION
/* The definition of JONJ_OCI8_VERSION changed in JONJ 5.3 and building
 * this code with JONJ 5.2 (e.g. when using OCI8 from PECL) will conflict.
 */
#undef JONJ_OCI8_VERSION
#endif
#define JONJ_OCI8_VERSION "2.0.12"

extern zend_module_entry oci8_module_entry;
#define jonjext_oci8_ptr &oci8_module_entry
#define jonjext_oci8_11g_ptr &oci8_module_entry


JONJ_MINIT_FUNCTION(oci);
JONJ_RINIT_FUNCTION(oci);
JONJ_MSHUTDOWN_FUNCTION(oci);
JONJ_RSHUTDOWN_FUNCTION(oci);
JONJ_MINFO_FUNCTION(oci);

# endif /* !JONJ_OCI8_H */
#else /* !HAVE_OCI8 */

# define oci8_module_ptr NULL

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
