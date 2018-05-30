/* Copyright 2009,2010 Ryan Dahl <ry@tinyclouds.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
/* modified by Moriyoshi Koizumi <moriyoshi@jonj.tk> to make it fit to JONJ source tree. */
#ifndef jonj_http_parser_h
#define jonj_http_parser_h
#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
# include <windows.h>
# include "config.w32.h"
#else
# include "jonj_config.h"
#endif

#include "jonj_stdint.h"

/* Compile with -DJONJ_HTTP_PARSER_STRICT=0 to make less checks, but run
 * faster
 */
#ifndef JONJ_HTTP_PARSER_STRICT
# define JONJ_HTTP_PARSER_STRICT 1
#else
# define JONJ_HTTP_PARSER_STRICT 0
#endif


/* Maximium header size allowed */
#define JONJ_HTTP_MAX_HEADER_SIZE (80*1024)


typedef struct jonj_http_parser jonj_http_parser;
typedef struct jonj_http_parser_settings jonj_http_parser_settings;


/* Callbacks should return non-zero to indicate an error. The parser will
 * then halt execution.
 *
 * The one exception is on_headers_complete. In a JONJ_HTTP_RESPONSE parser
 * returning '1' from on_headers_complete will tell the parser that it
 * should not expect a body. This is used when receiving a response to a
 * HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
 * chunked' headers that indicate the presence of a body.
 *
 * http_data_cb does not return data chunks. It will be call arbitrarally
 * many times for each string. E.G. you might get 10 callbacks for "on_path"
 * each providing just a few characters more data.
 */
typedef int (*jonj_http_data_cb) (jonj_http_parser*, const char *at, size_t length);
typedef int (*jonj_http_cb) (jonj_http_parser*);


/* Request Methods */
enum jonj_http_method
  { JONJ_HTTP_DELETE    = 0
  , JONJ_HTTP_GET
  , JONJ_HTTP_HEAD
  , JONJ_HTTP_POST
  , JONJ_HTTP_PUT
  , JONJ_HTTP_PATCH
  /* pathological */
  , JONJ_HTTP_CONNECT
  , JONJ_HTTP_OPTIONS
  , JONJ_HTTP_TRACE
  /* webdav */
  , JONJ_HTTP_COPY
  , JONJ_HTTP_LOCK
  , JONJ_HTTP_MKCOL
  , JONJ_HTTP_MOVE
  , JONJ_HTTP_MKCALENDAR
  , JONJ_HTTP_PROPFIND
  , JONJ_HTTP_PROPPATCH
  , JONJ_HTTP_UNLOCK
  /* subversion */
  , JONJ_HTTP_REPORT
  , JONJ_HTTP_MKACTIVITY
  , JONJ_HTTP_CHECKOUT
  , JONJ_HTTP_MERGE
  /* upnp */
  , JONJ_HTTP_MSEARCH
  , JONJ_HTTP_NOTIFY
  , JONJ_HTTP_SUBSCRIBE
  , JONJ_HTTP_UNSUBSCRIBE
  /* unknown, not implemented */
  , JONJ_HTTP_NOT_IMPLEMENTED
  };


enum jonj_http_parser_type { JONJ_HTTP_REQUEST, JONJ_HTTP_RESPONSE, JONJ_HTTP_BOTH };


struct jonj_http_parser {
  /** PRIVATE **/
  unsigned char type : 2;
  unsigned char flags : 6;
  unsigned char state;
  unsigned char header_state;
  unsigned char index;

  uint32_t nread;
  ssize_t  content_length;

  /** READ-ONLY **/
  unsigned short http_major;
  unsigned short http_minor;
  unsigned short status_code; /* responses only */
  unsigned char method;    /* requests only */

  /* 1 = Upgrade header was present and the parser has exited because of that.
   * 0 = No upgrade header present.
   * Should be checked when http_parser_execute() returns in addition to
   * error checking.
   */
  char upgrade;

  /** PUBLIC **/
  void *data; /* A pointer to get hook to the "connection" or "socket" object */
};


struct jonj_http_parser_settings {
  jonj_http_cb      on_message_begin;
  jonj_http_data_cb on_path;
  jonj_http_data_cb on_query_string;
  jonj_http_data_cb on_url;
  jonj_http_data_cb on_fragment;
  jonj_http_data_cb on_header_field;
  jonj_http_data_cb on_header_value;
  jonj_http_cb      on_headers_complete;
  jonj_http_data_cb on_body;
  jonj_http_cb      on_message_complete;
};


void jonj_http_parser_init(jonj_http_parser *parser, enum jonj_http_parser_type type);


size_t jonj_http_parser_execute(jonj_http_parser *parser,
                           const jonj_http_parser_settings *settings,
                           const char *data,
                           size_t len);


/* If jonj_http_should_keep_alive() in the on_headers_complete or
 * on_message_complete callback returns true, then this will be should be
 * the last message on the connection.
 * If you are the server, respond with the "Connection: close" header.
 * If you are the client, close the connection.
 */
int jonj_http_should_keep_alive(jonj_http_parser *parser);

/* Returns a string version of the HTTP method. */
const char *jonj_http_method_str(enum jonj_http_method);

#ifdef __cplusplus
}
#endif
#endif
