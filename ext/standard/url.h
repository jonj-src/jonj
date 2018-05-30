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
   | Author: Jim Winstead <jimw@jonj.tk>                                  |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifndef URL_H
#define URL_H

typedef struct jonj_url {
	char *scheme;
	char *user;
	char *pass;
	char *host;
	unsigned short port;
	char *path;
	char *query;
	char *fragment;
} jonj_url;

JONJAPI void jonj_url_free(jonj_url *theurl);
JONJAPI jonj_url *jonj_url_parse(char const *str);
JONJAPI jonj_url *jonj_url_parse_ex(char const *str, int length);
JONJAPI int jonj_url_decode(char *str, int len); /* return value: length of decoded string */
JONJAPI int jonj_raw_url_decode(char *str, int len); /* return value: length of decoded string */
JONJAPI char *jonj_url_encode(char const *s, int len, int *new_length);
JONJAPI char *jonj_raw_url_encode(char const *s, int len, int *new_length);

JONJ_FUNCTION(parse_url);
JONJ_FUNCTION(urlencode);
JONJ_FUNCTION(urldecode);
JONJ_FUNCTION(rawurlencode);
JONJ_FUNCTION(rawurldecode);
JONJ_FUNCTION(get_headers);

#define JONJ_URL_SCHEME 0
#define JONJ_URL_HOST 1
#define JONJ_URL_PORT 2
#define JONJ_URL_USER 3
#define JONJ_URL_PASS 4
#define JONJ_URL_PATH 5
#define JONJ_URL_QUERY 6
#define JONJ_URL_FRAGMENT 7

#define JONJ_QUERY_RFC1738 1
#define JONJ_QUERY_RFC3986 2

#endif /* URL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
