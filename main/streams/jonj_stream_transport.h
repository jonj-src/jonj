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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#ifdef JONJ_WIN32
#include "config.w32.h"
#include <Ws2tcpip.h>
#endif

#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

typedef jonj_stream *(jonj_stream_transport_factory_func)(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		jonj_stream_context *context STREAMS_DC TSRMLS_DC);
typedef jonj_stream_transport_factory_func *jonj_stream_transport_factory;

BEGIN_EXTERN_C()
JONJAPI int jonj_stream_xport_register(const char *protocol, jonj_stream_transport_factory factory TSRMLS_DC);
JONJAPI int jonj_stream_xport_unregister(const char *protocol TSRMLS_DC);

#define STREAM_XPORT_CLIENT			0
#define STREAM_XPORT_SERVER			1

#define STREAM_XPORT_CONNECT		2
#define STREAM_XPORT_BIND			4
#define STREAM_XPORT_LISTEN			8
#define STREAM_XPORT_CONNECT_ASYNC	16

/* Open a client or server socket connection */
JONJAPI jonj_stream *_jonj_stream_xport_create(const char *name, size_t namelen, int options,
		int flags, const char *persistent_id,
		struct timeval *timeout,
		jonj_stream_context *context,
		char **error_string,
		int *error_code
		STREAMS_DC TSRMLS_DC);

#define jonj_stream_xport_create(name, namelen, options, flags, persistent_id, timeout, context, estr, ecode) \
	_jonj_stream_xport_create(name, namelen, options, flags, persistent_id, timeout, context, estr, ecode STREAMS_CC TSRMLS_CC)

/* Bind the stream to a local address */
JONJAPI int jonj_stream_xport_bind(jonj_stream *stream,
		const char *name, size_t namelen,
		char **error_text
		TSRMLS_DC);

/* Connect to a remote address */
JONJAPI int jonj_stream_xport_connect(jonj_stream *stream,
		const char *name, size_t namelen,
		int asynchronous,
		struct timeval *timeout,
		char **error_text,
		int *error_code
		TSRMLS_DC);

/* Prepare to listen */
JONJAPI int jonj_stream_xport_listen(jonj_stream *stream,
		int backlog,
		char **error_text
		TSRMLS_DC);

/* Get the next client and their address as a string, or the underlying address
 * structure.  You must efree either of these if you request them */
JONJAPI int jonj_stream_xport_accept(jonj_stream *stream, jonj_stream **client,
		char **textaddr, int *textaddrlen,
		void **addr, socklen_t *addrlen,
		struct timeval *timeout,
		char **error_text
		TSRMLS_DC);

/* Get the name of either the socket or it's peer */
JONJAPI int jonj_stream_xport_get_name(jonj_stream *stream, int want_peer,
		char **textaddr, int *textaddrlen,
		void **addr, socklen_t *addrlen
		TSRMLS_DC);

enum jonj_stream_xport_send_recv_flags {
	STREAM_OOB = 1,
	STREAM_PEEK = 2
};

/* Similar to recv() system call; read data from the stream, optionally
 * peeking, optionally retrieving OOB data */
JONJAPI int jonj_stream_xport_recvfrom(jonj_stream *stream, char *buf, size_t buflen,
		long flags, void **addr, socklen_t *addrlen,
		char **textaddr, int *textaddrlen TSRMLS_DC);

/* Similar to send() system call; send data to the stream, optionally
 * sending it as OOB data */
JONJAPI int jonj_stream_xport_sendto(jonj_stream *stream, const char *buf, size_t buflen,
		long flags, void *addr, socklen_t addrlen TSRMLS_DC);

typedef enum {
	STREAM_SHUT_RD,
	STREAM_SHUT_WR,
	STREAM_SHUT_RDWR
} stream_shutdown_t;

/* Similar to shutdown() system call; shut down part of a full-duplex
 * connection */
JONJAPI int jonj_stream_xport_shutdown(jonj_stream *stream, stream_shutdown_t how TSRMLS_DC);
END_EXTERN_C()


/* Structure definition for the set_option interface that the above functions wrap */

typedef struct _jonj_stream_xport_param {
	enum {
		STREAM_XPORT_OP_BIND, STREAM_XPORT_OP_CONNECT,
		STREAM_XPORT_OP_LISTEN, STREAM_XPORT_OP_ACCEPT,
		STREAM_XPORT_OP_CONNECT_ASYNC,
		STREAM_XPORT_OP_GET_NAME,
		STREAM_XPORT_OP_GET_PEER_NAME,
		STREAM_XPORT_OP_RECV,
		STREAM_XPORT_OP_SEND,
		STREAM_XPORT_OP_SHUTDOWN
	} op;
	unsigned int want_addr:1;
	unsigned int want_textaddr:1;
	unsigned int want_errortext:1;
	unsigned int how:2;

	struct {
		char *name;
		size_t namelen;
		int backlog;
		struct timeval *timeout;
		struct sockaddr *addr;
		socklen_t addrlen;
		char *buf;
		size_t buflen;
		long flags;
	} inputs;
	struct {
		jonj_stream *client;
		int returncode;
		struct sockaddr *addr;
		socklen_t addrlen;
		char *textaddr;
		long textaddrlen;

		char *error_text;
		int error_code;
	} outputs;
} jonj_stream_xport_param;

/* Because both client and server streams use the same mechanisms
   for encryption we use the LSB to denote clients.
*/
typedef enum {
	STREAM_CRYPTO_METHOD_SSLv2_CLIENT = (1 << 1 | 1),
	STREAM_CRYPTO_METHOD_SSLv3_CLIENT = (1 << 2 | 1),
	/* v23 no longer negotiates SSL2 or SSL3 */
	STREAM_CRYPTO_METHOD_SSLv23_CLIENT = ((1 << 3) | (1 << 4) | (1 << 5) | 1),
	STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT = (1 << 3 | 1),
	STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT = (1 << 4 | 1),
	STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT = (1 << 5 | 1),
	/* tls now equates only to the specific TLSv1 method for BC with pre-5.6 */
	STREAM_CRYPTO_METHOD_TLS_CLIENT = (1 << 3 | 1),
	STREAM_CRYPTO_METHOD_TLS_ANY_CLIENT = ((1 << 3) | (1 << 4) | (1 << 5) | 1),
	STREAM_CRYPTO_METHOD_ANY_CLIENT = ((1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | 1),
	STREAM_CRYPTO_METHOD_SSLv2_SERVER = (1 << 1),
	STREAM_CRYPTO_METHOD_SSLv3_SERVER = (1 << 2),
	/* v23 no longer negotiates SSL2 or SSL3 */
	STREAM_CRYPTO_METHOD_SSLv23_SERVER = ((1 << 3) | (1 << 4) | (1 << 5)),
	STREAM_CRYPTO_METHOD_TLSv1_0_SERVER = (1 << 3),
	STREAM_CRYPTO_METHOD_TLSv1_1_SERVER = (1 << 4),
	STREAM_CRYPTO_METHOD_TLSv1_2_SERVER = (1 << 5),
	/* tls equates only to the specific TLSv1 method for BC with pre-5.6 */
	STREAM_CRYPTO_METHOD_TLS_SERVER = (1 << 3),
	STREAM_CRYPTO_METHOD_TLS_ANY_SERVER = ((1 << 3) | (1 << 4) | (1 << 5)),
	STREAM_CRYPTO_METHOD_ANY_SERVER = ((1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5))
} jonj_stream_xport_crypt_method_t;

/* These functions provide crypto support on the underlying transport */

BEGIN_EXTERN_C()
JONJAPI int jonj_stream_xport_crypto_setup(jonj_stream *stream, jonj_stream_xport_crypt_method_t crypto_method, jonj_stream *session_stream TSRMLS_DC);
JONJAPI int jonj_stream_xport_crypto_enable(jonj_stream *stream, int activate TSRMLS_DC);
END_EXTERN_C()

typedef struct _jonj_stream_xport_crypto_param {
	enum {
		STREAM_XPORT_CRYPTO_OP_SETUP,
		STREAM_XPORT_CRYPTO_OP_ENABLE
	} op;
	struct {
		int activate;
		jonj_stream_xport_crypt_method_t method;
		jonj_stream *session;
	} inputs;
	struct {
		int returncode;
	} outputs;
} jonj_stream_xport_crypto_param;

BEGIN_EXTERN_C()
JONJAPI HashTable *jonj_stream_xport_get_hash(void);
JONJAPI jonj_stream_transport_factory_func jonj_stream_generic_socket_factory;
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
