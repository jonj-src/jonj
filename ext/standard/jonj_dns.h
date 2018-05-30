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
   | Authors: The typical suspects                                        |
   |          Marcus Boerger <helly@jonj.tk>                              |
   |          Pollita <pollita@jonj.tk>                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_DNS_H
#define JONJ_DNS_H

#if defined(HAVE_DNS_SEARCH)
#define jonj_dns_search(res, dname, class, type, answer, anslen) \
    	((int)dns_search(res, dname, class, type, answer, anslen, (struct sockaddr *)&from, &fromsize))
#define jonj_dns_free_handle(res) \
		dns_free(res)

#elif defined(HAVE_RES_NSEARCH)
#define jonj_dns_search(res, dname, class, type, answer, anslen) \
			res_nsearch(res, dname, class, type, answer, anslen);
#define jonj_dns_free_handle(res) \
			res_nclose(res); \
			jonj_dns_free_res(*res)

#elif defined(HAVE_RES_SEARCH)
#define jonj_dns_search(res, dname, class, type, answer, anslen) \
			res_search(dname, class, type, answer, anslen)
#define jonj_dns_free_handle(res) /* noop */

#endif

#if defined(HAVE_DNS_SEARCH) || defined(HAVE_RES_NSEARCH) || defined(HAVE_RES_SEARCH)
#define HAVE_DNS_SEARCH_FUNC 1
#endif

#if HAVE_DNS_SEARCH_FUNC && HAVE_DN_EXPAND && HAVE_DN_SKIPNAME
#define HAVE_FULL_DNS_FUNCS 1
#endif

JONJ_FUNCTION(gethostbyaddr);
JONJ_FUNCTION(gethostbyname);
JONJ_FUNCTION(gethostbynamel);

#ifdef HAVE_GETHOSTNAME
JONJ_FUNCTION(gethostname);
#endif

#if defined(JONJ_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE)))
JONJ_FUNCTION(dns_check_record);

# if defined(JONJ_WIN32) || HAVE_FULL_DNS_FUNCS
JONJ_FUNCTION(dns_get_mx);
JONJ_FUNCTION(dns_get_record);
JONJ_MINIT_FUNCTION(dns);
# endif

#endif /* defined(JONJ_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE))) */

#ifndef INT16SZ
#define INT16SZ		2
#endif

#ifndef INT32SZ
#define INT32SZ		4
#endif

#endif /* JONJ_DNS_H */
