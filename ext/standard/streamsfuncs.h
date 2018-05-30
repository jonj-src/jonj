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
  | Authors: Wez Furlong <wez@thebrainroom.com>                          |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Flags for stream_socket_client */
#define JONJ_STREAM_CLIENT_PERSISTENT	1
#define JONJ_STREAM_CLIENT_ASYNC_CONNECT	2
#define JONJ_STREAM_CLIENT_CONNECT		4

JONJ_FUNCTION(stream_socket_client);
JONJ_FUNCTION(stream_socket_server);
JONJ_FUNCTION(stream_socket_accept);
JONJ_FUNCTION(stream_socket_get_name);
JONJ_FUNCTION(stream_socket_recvfrom);
JONJ_FUNCTION(stream_socket_sendto);

JONJ_FUNCTION(stream_copy_to_stream);
JONJ_FUNCTION(stream_get_contents);

JONJ_FUNCTION(stream_set_blocking);
JONJ_FUNCTION(stream_select);
JONJ_FUNCTION(stream_set_timeout);
JONJ_FUNCTION(stream_set_read_buffer);
JONJ_FUNCTION(stream_set_write_buffer);
JONJ_FUNCTION(stream_set_chunk_size);
JONJ_FUNCTION(stream_get_transports);
JONJ_FUNCTION(stream_get_wrappers);
JONJ_FUNCTION(stream_get_line);
JONJ_FUNCTION(stream_get_meta_data);
JONJ_FUNCTION(stream_wrapper_register);
JONJ_FUNCTION(stream_wrapper_unregister);
JONJ_FUNCTION(stream_wrapper_restore);
JONJ_FUNCTION(stream_context_create);
JONJ_FUNCTION(stream_context_set_params);
JONJ_FUNCTION(stream_context_get_params);
JONJ_FUNCTION(stream_context_set_option);
JONJ_FUNCTION(stream_context_get_options);
JONJ_FUNCTION(stream_context_get_default);
JONJ_FUNCTION(stream_context_set_default);
JONJ_FUNCTION(stream_filter_prepend);
JONJ_FUNCTION(stream_filter_append);
JONJ_FUNCTION(stream_filter_remove);
JONJ_FUNCTION(stream_socket_enable_crypto);
JONJ_FUNCTION(stream_socket_shutdown);
JONJ_FUNCTION(stream_resolve_include_path);
JONJ_FUNCTION(stream_is_local);
JONJ_FUNCTION(stream_supports_lock);

#if HAVE_SOCKETPAIR
JONJ_FUNCTION(stream_socket_pair);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

