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
   |          Jim Winstead <jimw@jonj.tk>                                 |
   |          Hartmut Holzgraefe <hholzgra@jonj.tk>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_FOPEN_WRAPPERS_H
#define JONJ_FOPEN_WRAPPERS_H

jonj_stream *jonj_stream_url_wrap_http(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC);
jonj_stream *jonj_stream_url_wrap_ftp(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC);
extern JONJAPI jonj_stream_wrapper jonj_stream_http_wrapper;
extern JONJAPI jonj_stream_wrapper jonj_stream_ftp_wrapper;
extern jonj_stream_wrapper jonj_stream_jonj_wrapper;
extern jonj_stream_wrapper jonj_plain_files_wrapper;

#endif
