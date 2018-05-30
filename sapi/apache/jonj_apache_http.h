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
  | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
  |          Stig Sæther Bakken <ssb@jonj.tk>                            |
  |          David Sklar <sklar@student.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#define NO_REGEX_EXTRA_H

#ifdef WIN32
#include <stddef.h>
#endif

#ifdef NETWARE
#include <netinet/in.h>
#endif

#include "zend.h"
#include "ext/ereg/jonj_regex.h"
#include "jonj_compat.h"

#ifdef HAVE_OPENSSL_EXT
/* zlib typedefs free_func which causes problems if the SSL includes happen
 * after zlib.h is included */
# include <openssl/ssl.h>
#endif

#ifdef regex_t
#undef regex_t
#endif

#include "httpd.h"
#include "http_config.h"

#if MODULE_MAGIC_NUMBER > 19980712
# include "ap_compat.h"
#else
# if MODULE_MAGIC_NUMBER > 19980324
#  include "compat.h"
# endif
#endif

#include "http_core.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_log.h"   
#include "util_script.h"

#include "jonj_variables.h"
#include "jonj_main.h"     
#include "jonj_ini.h"
#include "ext/standard/jonj_standard.h"

#include "mod_jonj5.h"
