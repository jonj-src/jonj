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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@jonj.tk>                         |
   |          Jason Greene     <jason@jonj.tk>                            |
   |          Gustavo Lopes    <cataphract@jonj.tk>                       |
   | WinSock: Daniel Beulshausen <daniel@jonj4win.de>                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"

#include "jonj.tkwork.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "jonj_ini.h"
#ifdef JONJ_WIN32
# include "windows_common.h"
# include <win32/inet.h>
# include <windows.h>
# include <Ws2tcpip.h>
# include "jonj_sockets.h"
# include <win32/sockets.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/un.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/uio.h>
# define IS_INVALID_SOCKET(a)	(a->bsd_socket < 0)
# define set_errno(a) (errno = a)
# include "jonj_sockets.h"
# if HAVE_IF_NAMETOINDEX
#  include <net/if.h>
# endif
#endif

#include <stddef.h>

#include "sockaddr_conv.h"
#include "multicast.h"
#include "sendrecvmsg.h"

ZEND_DECLARE_MODULE_GLOBALS(sockets)

#ifndef MSG_WAITALL
#ifdef LINUX
#define MSG_WAITALL 0x00000100
#else
#define MSG_WAITALL 0x00000000
#endif
#endif

#ifndef MSG_EOF
#ifdef MSG_FIN
#define MSG_EOF MSG_FIN
#endif
#endif

#ifndef SUN_LEN
#define SUN_LEN(su) (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

#ifndef PF_INET
#define PF_INET AF_INET
#endif

#define JONJ_NORMAL_READ 0x0001
#define JONJ_BINARY_READ 0x0002

static int le_socket;
#define le_socket_name jonj_sockets_le_socket_name

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_select, 0, 0, 4)
	ZEND_ARG_INFO(1, read_fds)
	ZEND_ARG_INFO(1, write_fds)
	ZEND_ARG_INFO(1, except_fds)
	ZEND_ARG_INFO(0, tv_sec)
	ZEND_ARG_INFO(0, tv_usec)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create_listen, 0, 0, 1)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_accept, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_set_nonblock, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_set_block, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_listen, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_close, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_write, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_read, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_getsockname, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, addr)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_getpeername, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, addr)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create, 0, 0, 3)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_connect, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_strerror, 0, 0, 1)
	ZEND_ARG_INFO(0, errno)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_bind, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_recv, 0, 0, 4)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_send, 0, 0, 4)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_recvfrom, 0, 0, 5)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(1, name)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_sendto, 0, 0, 5)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_get_option, 0, 0, 3)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, optname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_set_option, 0, 0, 4)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, optname)
	ZEND_ARG_INFO(0, optval)
ZEND_END_ARG_INFO()

#ifdef HAVE_SOCKETPAIR
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create_pair, 0, 0, 4)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, protocol)
	ZEND_ARG_INFO(1, fd)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_SHUTDOWN
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_shutdown, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, how)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_last_error, 0, 0, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_clear_error, 0, 0, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_import_stream, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_sendmsg, 0, 0, 3)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, msghdr)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_recvmsg, 0, 0, 3)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, msghdr)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_cmsg_space, 0, 0, 2)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()
/* }}} */

static JONJ_GINIT_FUNCTION(sockets);
static JONJ_MINIT_FUNCTION(sockets);
static JONJ_MSHUTDOWN_FUNCTION(sockets);
static JONJ_MINFO_FUNCTION(sockets);
static JONJ_RSHUTDOWN_FUNCTION(sockets);

JONJ_FUNCTION(socket_select);
JONJ_FUNCTION(socket_create_listen);
#ifdef HAVE_SOCKETPAIR
JONJ_FUNCTION(socket_create_pair);
#endif
JONJ_FUNCTION(socket_accept);
JONJ_FUNCTION(socket_set_nonblock);
JONJ_FUNCTION(socket_set_block);
JONJ_FUNCTION(socket_listen);
JONJ_FUNCTION(socket_close);
JONJ_FUNCTION(socket_write);
JONJ_FUNCTION(socket_read);
JONJ_FUNCTION(socket_getsockname);
JONJ_FUNCTION(socket_getpeername);
JONJ_FUNCTION(socket_create);
JONJ_FUNCTION(socket_connect);
JONJ_FUNCTION(socket_strerror);
JONJ_FUNCTION(socket_bind);
JONJ_FUNCTION(socket_recv);
JONJ_FUNCTION(socket_send);
JONJ_FUNCTION(socket_recvfrom);
JONJ_FUNCTION(socket_sendto);
JONJ_FUNCTION(socket_get_option);
JONJ_FUNCTION(socket_set_option);
#ifdef HAVE_SHUTDOWN
JONJ_FUNCTION(socket_shutdown);
#endif
JONJ_FUNCTION(socket_last_error);
JONJ_FUNCTION(socket_clear_error);
JONJ_FUNCTION(socket_import_stream);

/* {{{ sockets_functions[]
 */
const zend_function_entry sockets_functions[] = {
	JONJ_FE(socket_select,			arginfo_socket_select)
	JONJ_FE(socket_create,			arginfo_socket_create)
	JONJ_FE(socket_create_listen,	arginfo_socket_create_listen)
#ifdef HAVE_SOCKETPAIR
	JONJ_FE(socket_create_pair,		arginfo_socket_create_pair)
#endif
	JONJ_FE(socket_accept,			arginfo_socket_accept)
	JONJ_FE(socket_set_nonblock,		arginfo_socket_set_nonblock)
	JONJ_FE(socket_set_block,		arginfo_socket_set_block)
	JONJ_FE(socket_listen,			arginfo_socket_listen)
	JONJ_FE(socket_close,			arginfo_socket_close)
	JONJ_FE(socket_write,			arginfo_socket_write)
	JONJ_FE(socket_read,				arginfo_socket_read)
	JONJ_FE(socket_getsockname, 		arginfo_socket_getsockname)
	JONJ_FE(socket_getpeername, 		arginfo_socket_getpeername)
	JONJ_FE(socket_connect,			arginfo_socket_connect)
	JONJ_FE(socket_strerror,			arginfo_socket_strerror)
	JONJ_FE(socket_bind,				arginfo_socket_bind)
	JONJ_FE(socket_recv,				arginfo_socket_recv)
	JONJ_FE(socket_send,				arginfo_socket_send)
	JONJ_FE(socket_recvfrom,			arginfo_socket_recvfrom)
	JONJ_FE(socket_sendto,			arginfo_socket_sendto)
	JONJ_FE(socket_get_option,		arginfo_socket_get_option)
	JONJ_FE(socket_set_option,		arginfo_socket_set_option)
#ifdef HAVE_SHUTDOWN
	JONJ_FE(socket_shutdown,			arginfo_socket_shutdown)
#endif
	JONJ_FE(socket_last_error,		arginfo_socket_last_error)
	JONJ_FE(socket_clear_error,		arginfo_socket_clear_error)
	JONJ_FE(socket_import_stream,	arginfo_socket_import_stream)
	JONJ_FE(socket_sendmsg,			arginfo_socket_sendmsg)
	JONJ_FE(socket_recvmsg,			arginfo_socket_recvmsg)
	JONJ_FE(socket_cmsg_space,		arginfo_socket_cmsg_space)

	/* for downwards compatibility */
	JONJ_FALIAS(socket_getopt, socket_get_option, arginfo_socket_get_option)
	JONJ_FALIAS(socket_setopt, socket_set_option, arginfo_socket_set_option)

	JONJ_FE_END
};
/* }}} */

zend_module_entry sockets_module_entry = {
	STANDARD_MODULE_HEADER,
	"sockets",
	sockets_functions,
	JONJ_MINIT(sockets),
	JONJ_MSHUTDOWN(sockets),
	NULL,
	JONJ_RSHUTDOWN(sockets),
	JONJ_MINFO(sockets),
	NO_VERSION_YET,
	JONJ_MODULE_GLOBALS(sockets),
	JONJ_GINIT(sockets),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};


#ifdef COMPILE_DL_SOCKETS
ZEND_GET_MODULE(sockets)
#endif

/* inet_ntop should be used instead of inet_ntoa */
int inet_ntoa_lock = 0;

JONJ_SOCKETS_API int jonj_sockets_le_socket(void) /* {{{ */
{
	return le_socket;
}
/* }}} */

/* allocating function to make programming errors due to uninitialized fields
 * less likely */
static jonj_socket *jonj_create_socket(void) /* {{{ */
{
	jonj_socket *jonj_sock = emalloc(sizeof *jonj_sock);

	jonj_sock->bsd_socket = -1; /* invalid socket */
	jonj_sock->type		 = PF_UNSPEC;
	jonj_sock->error		 = 0;
	jonj_sock->blocking	 = 1;
	jonj_sock->zstream	 = NULL;

	return jonj_sock;
}
/* }}} */

static void jonj_destroy_socket(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	jonj_socket *jonj_sock = rsrc->ptr;

	if (jonj_sock->zstream == NULL) {
		if (!IS_INVALID_SOCKET(jonj_sock)) {
			close(jonj_sock->bsd_socket);
		}
	} else {
		zval_ptr_dtor(&jonj_sock->zstream);
	}
	efree(jonj_sock);
}
/* }}} */

static int jonj_open_listen_sock(jonj_socket **jonj_sock, int port, int backlog TSRMLS_DC) /* {{{ */
{
	struct sockaddr_in  la;
	struct hostent		*hp;
	jonj_socket			*sock = jonj_create_socket();

	*jonj_sock = sock;

#ifndef JONJ_WIN32
	if ((hp = gethostbyname("0.0.0.0")) == NULL) {
#else
	if ((hp = gethostbyname("localhost")) == NULL) {
#endif
		efree(sock);
		return 0;
	}

	memcpy((char *) &la.sin_addr, hp->h_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons((unsigned short) port);

	sock->bsd_socket = socket(PF_INET, SOCK_STREAM, 0);
	sock->blocking = 1;

	if (IS_INVALID_SOCKET(sock)) {
		JONJ_SOCKET_ERROR(sock, "unable to create listening socket", errno);
		efree(sock);
		return 0;
	}

	sock->type = PF_INET;

	if (bind(sock->bsd_socket, (struct sockaddr *)&la, sizeof(la)) != 0) {
		JONJ_SOCKET_ERROR(sock, "unable to bind to given address", errno);
		close(sock->bsd_socket);
		efree(sock);
		return 0;
	}

	if (listen(sock->bsd_socket, backlog) != 0) {
		JONJ_SOCKET_ERROR(sock, "unable to listen on socket", errno);
		close(sock->bsd_socket);
		efree(sock);
		return 0;
	}

	return 1;
}
/* }}} */

static int jonj_accept_connect(jonj_socket *in_sock, jonj_socket **new_sock, struct sockaddr *la, socklen_t *la_len TSRMLS_DC) /* {{{ */
{
	jonj_socket	*out_sock = jonj_create_socket();

	*new_sock = out_sock;

	out_sock->bsd_socket = accept(in_sock->bsd_socket, la, la_len);

	if (IS_INVALID_SOCKET(out_sock)) {
		JONJ_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
		efree(out_sock);
		return 0;
	}

	out_sock->error = 0;
	out_sock->blocking = 1;
	out_sock->type = la->sa_family;

	return 1;
}
/* }}} */

/* {{{ jonj_read -- wrapper around read() so that it only reads to a \r or \n. */
static int jonj_read(jonj_socket *sock, void *buf, size_t maxlen, int flags)
{
	int m = 0;
	size_t n = 0;
	int no_read = 0;
	int nonblock = 0;
	char *t = (char *) buf;

#ifndef JONJ_WIN32
	m = fcntl(sock->bsd_socket, F_GETFL);
	if (m < 0) {
		return m;
	}
	nonblock = (m & O_NONBLOCK);
	m = 0;
#else
	nonblock = !sock->blocking;
#endif
	set_errno(0);

	*t = '\0';
	while (*t != '\n' && *t != '\r' && n < maxlen) {
		if (m > 0) {
			t++;
			n++;
		} else if (m == 0) {
			no_read++;
			if (nonblock && no_read >= 2) {
				return n;
				/* The first pass, m always is 0, so no_read becomes 1
				 * in the first pass. no_read becomes 2 in the second pass,
				 * and if this is nonblocking, we should return.. */
			}

			if (no_read > 200) {
				set_errno(ECONNRESET);
				return -1;
			}
		}

		if (n < maxlen) {
			m = recv(sock->bsd_socket, (void *) t, 1, flags);
		}

		if (errno != 0 && errno != ESPIPE && errno != EAGAIN) {
			return -1;
		}

		set_errno(0);
	}

	if (n < maxlen) {
		n++;
		/* The only reasons it makes it to here is
		 * if '\n' or '\r' are encountered. So, increase
		 * the return by 1 to make up for the lack of the
		 * '\n' or '\r' in the count (since read() takes
		 * place at the end of the loop..) */
	}

	return n;
}
/* }}} */

char *sockets_strerror(int error TSRMLS_DC) /* {{{ */
{
	const char *buf;

#ifndef JONJ_WIN32
	if (error < -10000) {
		error = -error - 10000;

#ifdef HAVE_HSTRERROR
		buf = hstrerror(error);
#else
		{
			if (SOCKETS_G(strerror_buf)) {
				efree(SOCKETS_G(strerror_buf));
			}

			spprintf(&(SOCKETS_G(strerror_buf)), 0, "Host lookup error %d", error);
			buf = SOCKETS_G(strerror_buf);
		}
#endif
	} else {
		buf = strerror(error);
	}
#else
	{
		LPTSTR tmp = NULL;
		buf = NULL;

		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |	FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &tmp, 0, NULL)
		) {
			if (SOCKETS_G(strerror_buf)) {
				efree(SOCKETS_G(strerror_buf));
			}

			SOCKETS_G(strerror_buf) = estrdup(tmp);
			LocalFree(tmp);

			buf = SOCKETS_G(strerror_buf);
		}
	}
#endif

	return (buf ? (char *) buf : "");
}
/* }}} */

/* {{{ JONJ_GINIT_FUNCTION */
static JONJ_GINIT_FUNCTION(sockets)
{
	sockets_globals->last_error = 0;
	sockets_globals->strerror_buf = NULL;
}
/* }}} */

/* {{{ JONJ_MINIT_FUNCTION
 */
static JONJ_MINIT_FUNCTION(sockets)
{
	le_socket = zend_register_list_destructors_ex(jonj_destroy_socket, NULL, le_socket_name, module_number);

	REGISTER_LONG_CONSTANT("AF_UNIX",		AF_UNIX,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AF_INET",		AF_INET,		CONST_CS | CONST_PERSISTENT);
#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("AF_INET6",		AF_INET6,		CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SOCK_STREAM",	SOCK_STREAM,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_DGRAM",	SOCK_DGRAM,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RAW",		SOCK_RAW,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_SEQPACKET",SOCK_SEQPACKET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RDM",		SOCK_RDM,		CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("MSG_OOB",		MSG_OOB,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_WAITALL",	MSG_WAITALL,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_CTRUNC",	MSG_CTRUNC,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_TRUNC",		MSG_TRUNC,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_PEEK",		MSG_PEEK,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_DONTROUTE", MSG_DONTROUTE,	CONST_CS | CONST_PERSISTENT);
#ifdef MSG_EOR
	REGISTER_LONG_CONSTANT("MSG_EOR",		MSG_EOR,		CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_EOF
	REGISTER_LONG_CONSTANT("MSG_EOF",		MSG_EOF,		CONST_CS | CONST_PERSISTENT);
#endif

#ifdef MSG_CONFIRM
	REGISTER_LONG_CONSTANT("MSG_CONFIRM",	MSG_CONFIRM,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_ERRQUEUE
	REGISTER_LONG_CONSTANT("MSG_ERRQUEUE",	MSG_ERRQUEUE,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_NOSIGNAL
	REGISTER_LONG_CONSTANT("MSG_NOSIGNAL",	MSG_NOSIGNAL,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_DONTWAIT
	REGISTER_LONG_CONSTANT("MSG_DONTWAIT",	MSG_DONTWAIT,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_MORE
	REGISTER_LONG_CONSTANT("MSG_MORE",		MSG_MORE,		CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_WAITFORONE
	REGISTER_LONG_CONSTANT("MSG_WAITFORONE",MSG_WAITFORONE,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_CMSG_CLOEXEC
	REGISTER_LONG_CONSTANT("MSG_CMSG_CLOEXEC",MSG_CMSG_CLOEXEC,CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("SO_DEBUG",		SO_DEBUG,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_REUSEADDR",	SO_REUSEADDR,	CONST_CS | CONST_PERSISTENT);
#ifdef SO_REUSEPORT
	REGISTER_LONG_CONSTANT("SO_REUSEPORT",	SO_REUSEPORT,	CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SO_KEEPALIVE",	SO_KEEPALIVE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DONTROUTE",	SO_DONTROUTE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LINGER",		SO_LINGER,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_BROADCAST",	SO_BROADCAST,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_OOBINLINE",	SO_OOBINLINE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDBUF",		SO_SNDBUF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVBUF",		SO_RCVBUF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDLOWAT",	SO_SNDLOWAT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVLOWAT",	SO_RCVLOWAT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDTIMEO",	SO_SNDTIMEO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVTIMEO",	SO_RCVTIMEO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_TYPE",		SO_TYPE,		CONST_CS | CONST_PERSISTENT);
#ifdef SO_FAMILY
	REGISTER_LONG_CONSTANT("SO_FAMILY",		SO_FAMILY,		CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SO_ERROR",		SO_ERROR,		CONST_CS | CONST_PERSISTENT);
#ifdef SO_BINDTODEVICE
	REGISTER_LONG_CONSTANT("SO_BINDTODEVICE",       SO_BINDTODEVICE,        CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SOL_SOCKET",	SOL_SOCKET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOMAXCONN",		SOMAXCONN,		CONST_CS | CONST_PERSISTENT);
#ifdef TCP_NODELAY
	REGISTER_LONG_CONSTANT("TCP_NODELAY",   TCP_NODELAY,    CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("JONJ_NORMAL_READ", JONJ_NORMAL_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JONJ_BINARY_READ", JONJ_BINARY_READ, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("MCAST_JOIN_GROUP",			JONJ_MCAST_JOIN_GROUP,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_LEAVE_GROUP",			JONJ_MCAST_LEAVE_GROUP,			CONST_CS | CONST_PERSISTENT);
#ifdef HAS_MCAST_EXT
	REGISTER_LONG_CONSTANT("MCAST_BLOCK_SOURCE",		JONJ_MCAST_BLOCK_SOURCE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_UNBLOCK_SOURCE",		JONJ_MCAST_UNBLOCK_SOURCE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_JOIN_SOURCE_GROUP",	JONJ_MCAST_JOIN_SOURCE_GROUP,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_LEAVE_SOURCE_GROUP",	JONJ_MCAST_LEAVE_SOURCE_GROUP,	CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("IP_MULTICAST_IF",			IP_MULTICAST_IF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IP_MULTICAST_TTL",			IP_MULTICAST_TTL,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IP_MULTICAST_LOOP",			IP_MULTICAST_LOOP,		CONST_CS | CONST_PERSISTENT);
#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_MULTICAST_IF",			IPV6_MULTICAST_IF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_MULTICAST_HOPS",		IPV6_MULTICAST_HOPS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_MULTICAST_LOOP",		IPV6_MULTICAST_LOOP,	CONST_CS | CONST_PERSISTENT);
#endif

#ifndef WIN32
# include "unix_socket_constants.h"
#else
# include "win32_socket_constants.h"
#endif

	REGISTER_LONG_CONSTANT("IPPROTO_IP",	IPPROTO_IP,		CONST_CS | CONST_PERSISTENT);
#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPPROTO_IPV6",	IPPROTO_IPV6,	CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("SOL_TCP",		IPPROTO_TCP,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOL_UDP",		IPPROTO_UDP,	CONST_CS | CONST_PERSISTENT);

#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_UNICAST_HOPS",			IPV6_UNICAST_HOPS,	CONST_CS | CONST_PERSISTENT);
#endif

	jonj_socket_sendrecvmsg_init(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
static JONJ_MSHUTDOWN_FUNCTION(sockets)
{
	jonj_socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
static JONJ_MINFO_FUNCTION(sockets)
{
	jonj_info_print_table_start();
	jonj_info_print_table_row(2, "Sockets Support", "enabled");
	jonj_info_print_table_end();
}
/* }}} */

/* {{{ JONJ_RSHUTDOWN_FUNCTION */
static JONJ_RSHUTDOWN_FUNCTION(sockets)
{
	if (SOCKETS_G(strerror_buf)) {
		efree(SOCKETS_G(strerror_buf));
		SOCKETS_G(strerror_buf) = NULL;
	}

	return SUCCESS;
}
/* }}} */

static int jonj_sock_array_to_fd_set(zval *sock_array, fd_set *fds, JONJ_SOCKET *max_fd TSRMLS_DC) /* {{{ */
{
	zval		**element;
	jonj_socket	*jonj_sock;
	int			num = 0;

	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(sock_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(sock_array), (void **) &element) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(sock_array))) {

		jonj_sock = (jonj_socket*) zend_fetch_resource(element TSRMLS_CC, -1, le_socket_name, NULL, 1, le_socket);
		if (!jonj_sock) continue; /* If element is not a resource, skip it */

		JONJ_SAFE_FD_SET(jonj_sock->bsd_socket, fds);
		if (jonj_sock->bsd_socket > *max_fd) {
			*max_fd = jonj_sock->bsd_socket;
		}
		num++;
	}

	return num ? 1 : 0;
}
/* }}} */

static int jonj_sock_array_from_fd_set(zval *sock_array, fd_set *fds TSRMLS_DC) /* {{{ */
{
	zval		**element;
	zval		**dest_element;
	jonj_socket	*jonj_sock;
	HashTable	*new_hash;
	char 		*key;
	int			num = 0;
	ulong       num_key;
	uint 		key_len;

	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	ALLOC_HASHTABLE(new_hash);
	zend_hash_init(new_hash, zend_hash_num_elements(Z_ARRVAL_P(sock_array)), NULL, ZVAL_PTR_DTOR, 0);
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(sock_array));
		 zend_hash_get_current_data(Z_ARRVAL_P(sock_array), (void **) &element) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(sock_array))) {

		jonj_sock = (jonj_socket*) zend_fetch_resource(element TSRMLS_CC, -1, le_socket_name, NULL, 1, le_socket);
		if (!jonj_sock) continue; /* If element is not a resource, skip it */

		if (JONJ_SAFE_FD_ISSET(jonj_sock->bsd_socket, fds)) {
			/* Add fd to new array */
			switch (zend_hash_get_current_key_ex(Z_ARRVAL_P(sock_array), &key, &key_len, &num_key, 0, NULL)) {
				case HASH_KEY_IS_STRING:
					zend_hash_add(new_hash, key, key_len, (void *)element, sizeof(zval *), (void **)&dest_element);
					break;
				case HASH_KEY_IS_LONG:
					zend_hash_index_update(new_hash, num_key, (void *)element, sizeof(zval *), (void **)&dest_element);
					break;
			}
			if (dest_element) zval_add_ref(dest_element);
		}
		num++;
	}

	/* Destroy old array, add new one */
	zend_hash_destroy(Z_ARRVAL_P(sock_array));
	efree(Z_ARRVAL_P(sock_array));

	zend_hash_internal_pointer_reset(new_hash);
	Z_ARRVAL_P(sock_array) = new_hash;

	return num ? 1 : 0;
}
/* }}} */

/* {{{ proto int socket_select(array &read_fds, array &write_fds, array &except_fds, int tv_sec[, int tv_usec]) U
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
JONJ_FUNCTION(socket_select)
{
	zval			*r_array, *w_array, *e_array, *sec;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	JONJ_SOCKET		max_fd = 0;
	int				retval, sets = 0;
	long			usec = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a!a!a!z!|l", &r_array, &w_array, &e_array, &sec, &usec) == FAILURE) {
		return;
	}

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) sets += jonj_sock_array_to_fd_set(r_array, &rfds, &max_fd TSRMLS_CC);
	if (w_array != NULL) sets += jonj_sock_array_to_fd_set(w_array, &wfds, &max_fd TSRMLS_CC);
	if (e_array != NULL) sets += jonj_sock_array_to_fd_set(e_array, &efds, &max_fd TSRMLS_CC);

	if (!sets) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "no resource arrays were passed to select");
		RETURN_FALSE;
	}

	JONJ_SAFE_MAX_FD(max_fd, 0); /* someone needs to make this look more like stream_socket_select */

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (sec != NULL) {
		zval tmp;

		if (Z_TYPE_P(sec) != IS_LONG) {
			tmp = *sec;
			zval_copy_ctor(&tmp);
			convert_to_long(&tmp);
			sec = &tmp;
		}

		/* Solaris + BSD do not like microsecond values which are >= 1 sec */
		if (usec > 999999) {
			tv.tv_sec = Z_LVAL_P(sec) + (usec / 1000000);
			tv.tv_usec = usec % 1000000;
		} else {
			tv.tv_sec = Z_LVAL_P(sec);
			tv.tv_usec = usec;
		}

		tv_p = &tv;

		if (sec == &tmp) {
			zval_dtor(&tmp);
		}
	}

	retval = select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		SOCKETS_G(last_error) = errno;
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to select [%d]: %s", errno, sockets_strerror(errno TSRMLS_CC));
		RETURN_FALSE;
	}

	if (r_array != NULL) jonj_sock_array_from_fd_set(r_array, &rfds TSRMLS_CC);
	if (w_array != NULL) jonj_sock_array_from_fd_set(w_array, &wfds TSRMLS_CC);
	if (e_array != NULL) jonj_sock_array_from_fd_set(e_array, &efds TSRMLS_CC);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto resource socket_create_listen(int port[, int backlog]) U
   Opens a socket on port to accept connections */
JONJ_FUNCTION(socket_create_listen)
{
	jonj_socket	*jonj_sock;
	long		port, backlog = 128;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &port, &backlog) == FAILURE) {
		return;
	}

	if (!jonj_open_listen_sock(&jonj_sock, port, backlog TSRMLS_CC)) {
		RETURN_FALSE;
	}

	jonj_sock->error = 0;
	jonj_sock->blocking = 1;

	ZEND_REGISTER_RESOURCE(return_value, jonj_sock, le_socket);
}
/* }}} */

/* {{{ proto resource socket_accept(resource socket) U
   Accepts a connection on the listening socket fd */
JONJ_FUNCTION(socket_accept)
{
	zval				 *arg1;
	jonj_socket			 *jonj_sock, *new_sock;
	jonj_sockaddr_storage sa;
	socklen_t			 jonj_sa_len = sizeof(sa);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	if (!jonj_accept_connect(jonj_sock, &new_sock, (struct sockaddr*)&sa, &jonj_sa_len TSRMLS_CC)) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, new_sock, le_socket);
}
/* }}} */

/* {{{ proto bool socket_set_nonblock(resource socket) U
   Sets nonblocking mode on a socket resource */
JONJ_FUNCTION(socket_set_nonblock)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	if (jonj_sock->zstream != NULL) {
		jonj_stream *stream;
		/* omit notice if resource doesn't exist anymore */
		stream = zend_fetch_resource(&jonj_sock->zstream TSRMLS_CC, -1,
			NULL, NULL, 2, jonj_file_le_stream(), jonj_file_le_pstream());
		if (stream != NULL) {
			if (jonj_stream_set_option(stream, JONJ_STREAM_OPTION_BLOCKING, 0,
					NULL) != -1) {
				jonj_sock->blocking = 0;
				RETURN_TRUE;
			}
		}
	}

	if (jonj_set_sock_blocking(jonj_sock->bsd_socket, 0 TSRMLS_CC) == SUCCESS) {
		jonj_sock->blocking = 0;
		RETURN_TRUE;
	} else {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to set nonblocking mode", errno);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_set_block(resource socket) U
   Sets blocking mode on a socket resource */
JONJ_FUNCTION(socket_set_block)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	/* if socket was created from a stream, give the stream a chance to take
	 * care of the operation itself, thereby allowing it to update its internal
	 * state */
	if (jonj_sock->zstream != NULL) {
		jonj_stream *stream;
		stream = zend_fetch_resource(&jonj_sock->zstream TSRMLS_CC, -1,
			NULL, NULL, 2, jonj_file_le_stream(), jonj_file_le_pstream());
		if (stream != NULL) {
			if (jonj_stream_set_option(stream, JONJ_STREAM_OPTION_BLOCKING, 1,
					NULL) != -1) {
				jonj_sock->blocking = 1;
				RETURN_TRUE;
			}
		}
	}

	if (jonj_set_sock_blocking(jonj_sock->bsd_socket, 1 TSRMLS_CC) == SUCCESS) {
		jonj_sock->blocking = 1;
		RETURN_TRUE;
	} else {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to set blocking mode", errno);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_listen(resource socket[, int backlog]) U
   Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
JONJ_FUNCTION(socket_listen)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;
	long		backlog = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &arg1, &backlog) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	if (listen(jonj_sock->bsd_socket, backlog) != 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to listen on socket", errno);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void socket_close(resource socket) U
   Closes a file descriptor */
JONJ_FUNCTION(socket_close)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);
	if (jonj_sock->zstream != NULL) {
		jonj_stream *stream = NULL;
		jonj_stream_from_zval_no_verify(stream, &jonj_sock->zstream);
		if (stream != NULL) {
			/* close & destroy stream, incl. removing it from the rsrc list;
			 * resource stored in jonj_sock->zstream will become invalid */
			jonj_stream_free(stream, JONJ_STREAM_FREE_CLOSE |
					(stream->is_persistent?JONJ_STREAM_FREE_CLOSE_PERSISTENT:0));
		}
	}
	zend_list_delete(Z_RESVAL_P(arg1));
}
/* }}} */

/* {{{ proto int socket_write(resource socket, string buf[, int length])
   Writes the buffer to the socket resource, length is optional */
JONJ_FUNCTION(socket_write)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;
	int			retval, str_len;
	long		length = 0;
	char		*str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &arg1, &str, &str_len, &length) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	if (ZEND_NUM_ARGS() < 3) {
		length = str_len;
	}

#ifndef JONJ_WIN32
	retval = write(jonj_sock->bsd_socket, str, MIN(length, str_len));
#else
	retval = send(jonj_sock->bsd_socket, str, min(length, str_len), 0);
#endif

	if (retval < 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string socket_read(resource socket, int length [, int type]) U
   Reads a maximum of length bytes from socket */
JONJ_FUNCTION(socket_read)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;
	char		*tmpbuf;
	int			retval;
	long		length, type = JONJ_BINARY_READ;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", &arg1, &length, &type) == FAILURE) {
		return;
	}

	/* overflow check */
	if ((length + 1) < 2) {
		RETURN_FALSE;
	}

	tmpbuf = emalloc(length + 1);

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	if (type == JONJ_NORMAL_READ) {
		retval = jonj_read(jonj_sock, tmpbuf, length, 0);
	} else {
		retval = recv(jonj_sock->bsd_socket, tmpbuf, length, 0);
	}

	if (retval == -1) {
		/* if the socket is in non-blocking mode and there's no data to read,
		don't output any error, as this is a normal situation, and not an error */
		if (errno == EAGAIN
#ifdef EWOULDBLOCK
		|| errno == EWOULDBLOCK
#endif
		) {
			jonj_sock->error = errno;
			SOCKETS_G(last_error) = errno;
		} else {
			JONJ_SOCKET_ERROR(jonj_sock, "unable to read from socket", errno);
		}

		efree(tmpbuf);
		RETURN_FALSE;
	} else if (!retval) {
		efree(tmpbuf);
		RETURN_EMPTY_STRING();
	}

	tmpbuf = erealloc(tmpbuf, retval + 1);
	tmpbuf[retval] = '\0' ;

	RETURN_STRINGL(tmpbuf, retval, 0);
}
/* }}} */

/* {{{ proto bool socket_getsockname(resource socket, string &addr[, int &port])
   Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
JONJ_FUNCTION(socket_getsockname)
{
	zval					*arg1, *addr, *port = NULL;
	jonj_sockaddr_storage	sa_storage;
	jonj_socket				*jonj_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#if HAVE_IPV6
	struct sockaddr_in6		*sin6;
	char					addr6[INET6_ADDRSTRLEN+1];
#endif
	struct sockaddr_un		*s_un;
	char					*addr_string;
	socklen_t				salen = sizeof(jonj_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|z", &arg1, &addr, &port) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	sa = (struct sockaddr *) &sa_storage;

	if (getsockname(jonj_sock->bsd_socket, sa, &salen) != 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve socket name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
#if HAVE_IPV6
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *) sa;
			inet_ntop(AF_INET6, &sin6->sin6_addr, addr6, INET6_ADDRSTRLEN);
			zval_dtor(addr);
			ZVAL_STRING(addr, addr6, 1);

			if (port != NULL) {
				zval_dtor(port);
				ZVAL_LONG(port, htons(sin6->sin6_port));
			}
			RETURN_TRUE;
			break;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;

			zval_dtor(addr);
			ZVAL_STRING(addr, addr_string, 1);

			if (port != NULL) {
				zval_dtor(port);
				ZVAL_LONG(port, htons(sin->sin_port));
			}
			RETURN_TRUE;
			break;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			zval_dtor(addr);
			ZVAL_STRING(addr, s_un->sun_path, 1);
			RETURN_TRUE;
			break;

		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported address family %d", sa->sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_getpeername(resource socket, string &addr[, int &port])
   Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
JONJ_FUNCTION(socket_getpeername)
{
	zval					*arg1, *arg2, *arg3 = NULL;
	jonj_sockaddr_storage	sa_storage;
	jonj_socket				*jonj_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#if HAVE_IPV6
	struct sockaddr_in6		*sin6;
	char					addr6[INET6_ADDRSTRLEN+1];
#endif
	struct sockaddr_un		*s_un;
	char					*addr_string;
	socklen_t				salen = sizeof(jonj_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|z", &arg1, &arg2, &arg3) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	sa = (struct sockaddr *) &sa_storage;

	if (getpeername(jonj_sock->bsd_socket, sa, &salen) < 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve peer name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
#if HAVE_IPV6
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *) sa;
			inet_ntop(AF_INET6, &sin6->sin6_addr, addr6, INET6_ADDRSTRLEN);
			zval_dtor(arg2);
			ZVAL_STRING(arg2, addr6, 1);

			if (arg3 != NULL) {
				zval_dtor(arg3);
				ZVAL_LONG(arg3, htons(sin6->sin6_port));
			}

			RETURN_TRUE;
			break;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;

			zval_dtor(arg2);
			ZVAL_STRING(arg2, addr_string, 1);

			if (arg3 != NULL) {
				zval_dtor(arg3);
				ZVAL_LONG(arg3, htons(sin->sin_port));
			}

			RETURN_TRUE;
			break;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			zval_dtor(arg2);
			ZVAL_STRING(arg2, s_un->sun_path, 1);
			RETURN_TRUE;
			break;

		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported address family %d", sa->sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource socket_create(int domain, int type, int protocol) U
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
JONJ_FUNCTION(socket_create)
{
	long		arg1, arg2, arg3;
	jonj_socket	*jonj_sock = jonj_create_socket();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &arg1, &arg2, &arg3) == FAILURE) {
		efree(jonj_sock);
		return;
	}

	if (arg1 != AF_UNIX
#if HAVE_IPV6
		&& arg1 != AF_INET6
#endif
		&& arg1 != AF_INET) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid socket domain [%ld] specified for argument 1, assuming AF_INET", arg1);
		arg1 = AF_INET;
	}

	if (arg2 > 10) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid socket type [%ld] specified for argument 2, assuming SOCK_STREAM", arg2);
		arg2 = SOCK_STREAM;
	}

	jonj_sock->bsd_socket = socket(arg1, arg2, arg3);
	jonj_sock->type = arg1;

	if (IS_INVALID_SOCKET(jonj_sock)) {
		SOCKETS_G(last_error) = errno;
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno TSRMLS_CC));
		efree(jonj_sock);
		RETURN_FALSE;
	}

	jonj_sock->error = 0;
	jonj_sock->blocking = 1;

	ZEND_REGISTER_RESOURCE(return_value, jonj_sock, le_socket);
}
/* }}} */

/* {{{ proto bool socket_connect(resource socket, string addr [, int port])
   Opens a connection to addr:port on the socket specified by socket */
JONJ_FUNCTION(socket_connect)
{
	zval				*arg1;
	jonj_socket			*jonj_sock;
	char				*addr;
	int					retval, addr_len;
	long				port = 0;
	int					argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rs|l", &arg1, &addr, &addr_len, &port) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	switch(jonj_sock->type) {
#if HAVE_IPV6
		case AF_INET6: {
			struct sockaddr_in6 sin6 = {0};

			if (argc != 3) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Socket of type AF_INET6 requires 3 arguments");
				RETURN_FALSE;
			}

			memset(&sin6, 0, sizeof(struct sockaddr_in6));

			sin6.sin6_family = AF_INET6;
			sin6.sin6_port   = htons((unsigned short int)port);

			if (! jonj_set_inet6_addr(&sin6, addr, jonj_sock TSRMLS_CC)) {
				RETURN_FALSE;
			}

			retval = connect(jonj_sock->bsd_socket, (struct sockaddr *)&sin6, sizeof(struct sockaddr_in6));
			break;
		}
#endif
		case AF_INET: {
			struct sockaddr_in sin = {0};

			if (argc != 3) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Socket of type AF_INET requires 3 arguments");
				RETURN_FALSE;
			}

			sin.sin_family = AF_INET;
			sin.sin_port   = htons((unsigned short int)port);

			if (! jonj_set_inet_addr(&sin, addr, jonj_sock TSRMLS_CC)) {
				RETURN_FALSE;
			}

			retval = connect(jonj_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
			break;
		}

		case AF_UNIX: {
			struct sockaddr_un s_un = {0};

			if (addr_len >= sizeof(s_un.sun_path)) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Path too long");
				RETURN_FALSE;
			}

			s_un.sun_family = AF_UNIX;
			memcpy(&s_un.sun_path, addr, addr_len);
			retval = connect(jonj_sock->bsd_socket, (struct sockaddr *) &s_un,
				(socklen_t)(XtOffsetOf(struct sockaddr_un, sun_path) + addr_len));
			break;
		}

		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported socket type %d", jonj_sock->type);
			RETURN_FALSE;
		}

	if (retval != 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to connect", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string socket_strerror(int errno)
   Returns a string describing an error */
JONJ_FUNCTION(socket_strerror)
{
	long	arg1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &arg1) == FAILURE) {
		return;
	}

	RETURN_STRING(sockets_strerror(arg1 TSRMLS_CC), 1);
}
/* }}} */

/* {{{ proto bool socket_bind(resource socket, string addr [, int port])
   Binds an open socket to a listening port, port is only specified in AF_INET family. */
JONJ_FUNCTION(socket_bind)
{
	zval					*arg1;
	jonj_sockaddr_storage	sa_storage = {0};
	struct sockaddr			*sock_type = (struct sockaddr*) &sa_storage;
	jonj_socket				*jonj_sock;
	char					*addr;
	int						addr_len;
	long					port = 0;
	long					retval = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &arg1, &addr, &addr_len, &port) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	switch(jonj_sock->type) {
		case AF_UNIX:
			{
				struct sockaddr_un *sa = (struct sockaddr_un *) sock_type;

				sa->sun_family = AF_UNIX;

				if (addr_len >= sizeof(sa->sun_path)) {
					jonj_error_docref(NULL TSRMLS_CC, E_WARNING,
							"Invalid path: too long (maximum size is %d)",
							(int)sizeof(sa->sun_path) - 1);
					RETURN_FALSE;
				}
				memcpy(&sa->sun_path, addr, addr_len);

				retval = bind(jonj_sock->bsd_socket, (struct sockaddr *) sa,
						offsetof(struct sockaddr_un, sun_path) + addr_len);
				break;
			}

		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) sock_type;

				sa->sin_family = AF_INET;
				sa->sin_port = htons((unsigned short) port);

				if (! jonj_set_inet_addr(sa, addr, jonj_sock TSRMLS_CC)) {
					RETURN_FALSE;
				}

				retval = bind(jonj_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in));
				break;
			}
#if HAVE_IPV6
		case AF_INET6:
			{
				struct sockaddr_in6 *sa = (struct sockaddr_in6 *) sock_type;

				sa->sin6_family = AF_INET6;
				sa->sin6_port = htons((unsigned short) port);

				if (! jonj_set_inet6_addr(sa, addr, jonj_sock TSRMLS_CC)) {
					RETURN_FALSE;
				}

				retval = bind(jonj_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in6));
				break;
			}
#endif
		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "unsupported socket type '%d', must be AF_UNIX, AF_INET, or AF_INET6", jonj_sock->type);
			RETURN_FALSE;
	}

	if (retval != 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to bind address", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int socket_recv(resource socket, string &buf, int len, int flags)
   Receives data from a connected socket */
JONJ_FUNCTION(socket_recv)
{
	zval		*jonj_sock_res, *buf;
	char		*recv_buf;
	jonj_socket	*jonj_sock;
	int			retval;
	long		len, flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzll", &jonj_sock_res, &buf, &len, &flags) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &jonj_sock_res, -1, le_socket_name, le_socket);

	/* overflow check */
	if ((len + 1) < 2) {
		RETURN_FALSE;
	}

	recv_buf = emalloc(len + 1);
	memset(recv_buf, 0, len + 1);

	if ((retval = recv(jonj_sock->bsd_socket, recv_buf, len, flags)) < 1) {
		efree(recv_buf);

		zval_dtor(buf);
		Z_TYPE_P(buf) = IS_NULL;
	} else {
		recv_buf[retval] = '\0';

		/* Rebuild buffer zval */
		zval_dtor(buf);

		Z_STRVAL_P(buf) = recv_buf;
		Z_STRLEN_P(buf) = retval;
		Z_TYPE_P(buf) = IS_STRING;
	}

	if (retval == -1) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to read from socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_send(resource socket, string buf, int len, int flags)
   Sends data to a connected socket */
JONJ_FUNCTION(socket_send)
{
	zval		*arg1;
	jonj_socket	*jonj_sock;
	int			buf_len, retval;
	long		len, flags;
	char		*buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsll", &arg1, &buf, &buf_len, &len, &flags) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	retval = send(jonj_sock->bsd_socket, buf, (buf_len < len ? buf_len : len), flags);

	if (retval == -1) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_recvfrom(resource socket, string &buf, int len, int flags, string &name [, int &port])
   Receives data from a socket, connected or not */
JONJ_FUNCTION(socket_recvfrom)
{
	zval				*arg1, *arg2, *arg5, *arg6 = NULL;
	jonj_socket			*jonj_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#if HAVE_IPV6
	struct sockaddr_in6	sin6;
	char				addr6[INET6_ADDRSTRLEN];
#endif
	socklen_t			slen;
	int					retval;
	long				arg3, arg4;
	char				*recv_buf, *address;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzllz|z", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	/* overflow check */
	if ((arg3 + 2) < 3) {
		RETURN_FALSE;
	}

	recv_buf = emalloc(arg3 + 2);
	memset(recv_buf, 0, arg3 + 2);

	switch (jonj_sock->type) {
		case AF_UNIX:
			slen = sizeof(s_un);
			s_un.sun_family = AF_UNIX;
			retval = recvfrom(jonj_sock->bsd_socket, recv_buf, arg3, arg4, (struct sockaddr *)&s_un, (socklen_t *)&slen);

			if (retval < 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to recvfrom", errno);
				efree(recv_buf);
				RETURN_FALSE;
			}

			zval_dtor(arg2);
			zval_dtor(arg5);

			ZVAL_STRINGL(arg2, recv_buf, retval, 0);
			ZVAL_STRING(arg5, s_un.sun_path, 1);
			break;

		case AF_INET:
			slen = sizeof(sin);
			memset(&sin, 0, slen);
			sin.sin_family = AF_INET;

			if (arg6 == NULL) {
				efree(recv_buf);
				WRONG_PARAM_COUNT;
			}

			retval = recvfrom(jonj_sock->bsd_socket, recv_buf, arg3, arg4, (struct sockaddr *)&sin, (socklen_t *)&slen);

			if (retval < 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to recvfrom", errno);
				efree(recv_buf);
				RETURN_FALSE;
			}

			zval_dtor(arg2);
			zval_dtor(arg5);
			zval_dtor(arg6);

			address = inet_ntoa(sin.sin_addr);

			ZVAL_STRINGL(arg2, recv_buf, retval, 0);
			ZVAL_STRING(arg5, address ? address : "0.0.0.0", 1);
			ZVAL_LONG(arg6, ntohs(sin.sin_port));
			break;
#if HAVE_IPV6
		case AF_INET6:
			slen = sizeof(sin6);
			memset(&sin6, 0, slen);
			sin6.sin6_family = AF_INET6;

			if (arg6 == NULL) {
				efree(recv_buf);
				WRONG_PARAM_COUNT;
			}

			retval = recvfrom(jonj_sock->bsd_socket, recv_buf, arg3, arg4, (struct sockaddr *)&sin6, (socklen_t *)&slen);

			if (retval < 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to recvfrom", errno);
				efree(recv_buf);
				RETURN_FALSE;
			}

			zval_dtor(arg2);
			zval_dtor(arg5);
			zval_dtor(arg6);

			memset(addr6, 0, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);

			ZVAL_STRINGL(arg2, recv_buf, retval, 0);
			ZVAL_STRING(arg5, addr6[0] ? addr6 : "::", 1);
			ZVAL_LONG(arg6, ntohs(sin6.sin6_port));
			break;
#endif
		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported socket type %d", jonj_sock->type);
			RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_sendto(resource socket, string buf, int len, int flags, string addr [, int port])
   Sends a message to a socket, whether it is connected or not */
JONJ_FUNCTION(socket_sendto)
{
	zval				*arg1;
	jonj_socket			*jonj_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#if HAVE_IPV6
	struct sockaddr_in6	sin6;
#endif
	int					retval, buf_len, addr_len;
	long				len, flags, port = 0;
	char				*buf, *addr;
	int					argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rslls|l", &arg1, &buf, &buf_len, &len, &flags, &addr, &addr_len, &port) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	switch (jonj_sock->type) {
		case AF_UNIX:
			memset(&s_un, 0, sizeof(s_un));
			s_un.sun_family = AF_UNIX;
			snprintf(s_un.sun_path, 108, "%s", addr);

			retval = sendto(jonj_sock->bsd_socket, buf, (len > buf_len) ? buf_len : len,	flags, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
			break;

		case AF_INET:
			if (argc != 6) {
				WRONG_PARAM_COUNT;
			}

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_port = htons((unsigned short) port);

			if (! jonj_set_inet_addr(&sin, addr, jonj_sock TSRMLS_CC)) {
				RETURN_FALSE;
			}

			retval = sendto(jonj_sock->bsd_socket, buf, (len > buf_len) ? buf_len : len, flags, (struct sockaddr *) &sin, sizeof(sin));
			break;
#if HAVE_IPV6
		case AF_INET6:
			if (argc != 6) {
				WRONG_PARAM_COUNT;
			}

			memset(&sin6, 0, sizeof(sin6));
			sin6.sin6_family = AF_INET6;
			sin6.sin6_port = htons((unsigned short) port);

			if (! jonj_set_inet6_addr(&sin6, addr, jonj_sock TSRMLS_CC)) {
				RETURN_FALSE;
			}

			retval = sendto(jonj_sock->bsd_socket, buf, (len > buf_len) ? buf_len : len, flags, (struct sockaddr *) &sin6, sizeof(sin6));
			break;
#endif
		default:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported socket type %d", jonj_sock->type);
			RETURN_FALSE;
	}

	if (retval == -1) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto mixed socket_get_option(resource socket, int level, int optname) U
   Gets socket options for the socket */
JONJ_FUNCTION(socket_get_option)
{
	zval			*arg1;
	struct linger	linger_val;
	struct timeval	tv;
#ifdef JONJ_WIN32
	int				timeout = 0;
#endif
	socklen_t		optlen;
	jonj_socket		*jonj_sock;
	int				other_val;
	long			level, optname;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &arg1, &level, &optname) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	if (level == IPPROTO_IP) {
		switch (optname) {
		case IP_MULTICAST_IF: {
			struct in_addr if_addr;
			unsigned int if_index;
			optlen = sizeof(if_addr);
			if (getsockopt(jonj_sock->bsd_socket, level, optname, (char*)&if_addr, &optlen) != 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}
			if (jonj_add4_to_if_index(&if_addr, jonj_sock, &if_index TSRMLS_CC) == SUCCESS) {
				RETURN_LONG((long) if_index);
			} else {
				RETURN_FALSE;
			}
		}
		}
	}
#if HAVE_IPV6
	else if (level == IPPROTO_IPV6) {
		int ret = jonj_do_getsockopt_ipv6_rfc3542(jonj_sock, level, optname, return_value TSRMLS_CC);
		if (ret == SUCCESS) {
			return;
		} else if (ret == FAILURE) {
			RETURN_FALSE;
		} /* else continue */
	}
#endif

	/* sol_socket options and general case */
	switch(optname) {
		case SO_LINGER:
			optlen = sizeof(linger_val);

			if (getsockopt(jonj_sock->bsd_socket, level, optname, (char*)&linger_val, &optlen) != 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}

			array_init(return_value);
			add_assoc_long(return_value, "l_onoff", linger_val.l_onoff);
			add_assoc_long(return_value, "l_linger", linger_val.l_linger);
			break;

		case SO_RCVTIMEO:
		case SO_SNDTIMEO:
#ifndef JONJ_WIN32
			optlen = sizeof(tv);

			if (getsockopt(jonj_sock->bsd_socket, level, optname, (char*)&tv, &optlen) != 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}
#else
			optlen = sizeof(int);

			if (getsockopt(jonj_sock->bsd_socket, level, optname, (char*)&timeout, &optlen) != 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}

			tv.tv_sec = timeout ? timeout / 1000 : 0;
			tv.tv_usec = timeout ? (timeout * 1000) % 1000000 : 0;
#endif

			array_init(return_value);

			add_assoc_long(return_value, "sec", tv.tv_sec);
			add_assoc_long(return_value, "usec", tv.tv_usec);
			break;

		default:
			optlen = sizeof(other_val);

			if (getsockopt(jonj_sock->bsd_socket, level, optname, (char*)&other_val, &optlen) != 0) {
				JONJ_SOCKET_ERROR(jonj_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}
			if (optlen == 1)
				other_val = *((unsigned char *)&other_val);

			RETURN_LONG(other_val);
			break;
	}
}
/* }}} */

/* {{{ proto bool socket_set_option(resource socket, int level, int optname, int|array optval)
   Sets socket options for the socket */
JONJ_FUNCTION(socket_set_option)
{
	zval					*arg1, **arg4;
	struct linger			lv;
	jonj_socket				*jonj_sock;
	int						ov, optlen, retval;
#ifdef JONJ_WIN32
	int						timeout;
#else
	struct					timeval tv;
#endif
	long					level, optname;
	void 					*opt_ptr;
	HashTable		 		*opt_ht;
	zval 					**l_onoff, **l_linger;
	zval		 			**sec, **usec;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllZ", &arg1, &level, &optname, &arg4) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket *, &arg1, -1, le_socket_name, le_socket);

	set_errno(0);

#define HANDLE_SUBCALL(res) \
	do { \
		if (res == 1) { goto default_case; } \
		else if (res == SUCCESS) { RETURN_TRUE; } \
		else { RETURN_FALSE; } \
	} while (0)


	if (level == IPPROTO_IP) {
		int res = jonj_do_setsockopt_ip_mcast(jonj_sock, level, optname, arg4 TSRMLS_CC);
		HANDLE_SUBCALL(res);
	}

#if HAVE_IPV6
	else if (level == IPPROTO_IPV6) {
		int res = jonj_do_setsockopt_ipv6_mcast(jonj_sock, level, optname, arg4 TSRMLS_CC);
		if (res == 1) {
			res = jonj_do_setsockopt_ipv6_rfc3542(jonj_sock, level, optname, arg4 TSRMLS_CC);
		}
		HANDLE_SUBCALL(res);
	}
#endif

	switch (optname) {
		case SO_LINGER: {
			const char l_onoff_key[] = "l_onoff";
			const char l_linger_key[] = "l_linger";

			convert_to_array_ex(arg4);
			opt_ht = HASH_OF(*arg4);

			if (zend_hash_find(opt_ht, l_onoff_key, sizeof(l_onoff_key), (void **)&l_onoff) == FAILURE) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "no key \"%s\" passed in optval", l_onoff_key);
				RETURN_FALSE;
			}
			if (zend_hash_find(opt_ht, l_linger_key, sizeof(l_linger_key), (void **)&l_linger) == FAILURE) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "no key \"%s\" passed in optval", l_linger_key);
				RETURN_FALSE;
			}

			convert_to_long_ex(l_onoff);
			convert_to_long_ex(l_linger);

			lv.l_onoff = (unsigned short)Z_LVAL_PP(l_onoff);
			lv.l_linger = (unsigned short)Z_LVAL_PP(l_linger);

			optlen = sizeof(lv);
			opt_ptr = &lv;
			break;
		}

		case SO_RCVTIMEO:
		case SO_SNDTIMEO: {
			const char sec_key[] = "sec";
			const char usec_key[] = "usec";

			convert_to_array_ex(arg4);
			opt_ht = HASH_OF(*arg4);

			if (zend_hash_find(opt_ht, sec_key, sizeof(sec_key), (void **)&sec) == FAILURE) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "no key \"%s\" passed in optval", sec_key);
				RETURN_FALSE;
			}
			if (zend_hash_find(opt_ht, usec_key, sizeof(usec_key), (void **)&usec) == FAILURE) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "no key \"%s\" passed in optval", usec_key);
				RETURN_FALSE;
			}

			convert_to_long_ex(sec);
			convert_to_long_ex(usec);
#ifndef JONJ_WIN32
			tv.tv_sec = Z_LVAL_PP(sec);
			tv.tv_usec = Z_LVAL_PP(usec);
			optlen = sizeof(tv);
			opt_ptr = &tv;
#else
			timeout = Z_LVAL_PP(sec) * 1000 + Z_LVAL_PP(usec) / 1000;
			optlen = sizeof(int);
			opt_ptr = &timeout;
#endif
			break;
		}
#ifdef SO_BINDTODEVICE
		case SO_BINDTODEVICE: {
			if (Z_TYPE_PP(arg4) == IS_STRING) {
				opt_ptr = Z_STRVAL_PP(arg4);
				optlen = Z_STRLEN_PP(arg4);
			} else {
				opt_ptr = "";
				optlen = 0;
			}
			break;
		}
#endif

		default:
default_case:
			convert_to_long_ex(arg4);
			ov = Z_LVAL_PP(arg4);

			optlen = sizeof(ov);
			opt_ptr = &ov;
			break;
	}

	retval = setsockopt(jonj_sock->bsd_socket, level, optname, opt_ptr, optlen);
	if (retval != 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to set socket option", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_SOCKETPAIR
/* {{{ proto bool socket_create_pair(int domain, int type, int protocol, array &fd) U
   Creates a pair of indistinguishable sockets and stores them in fds. */
JONJ_FUNCTION(socket_create_pair)
{
	zval		*retval[2], *fds_array_zval;
	jonj_socket	*jonj_sock[2];
	JONJ_SOCKET	fds_array[2];
	long		domain, type, protocol;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllz", &domain, &type, &protocol, &fds_array_zval) == FAILURE) {
		return;
	}

	jonj_sock[0] = jonj_create_socket();
	jonj_sock[1] = jonj_create_socket();

	if (domain != AF_INET
#if HAVE_IPV6
		&& domain != AF_INET6
#endif
		&& domain != AF_UNIX) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid socket domain [%ld] specified for argument 1, assuming AF_INET", domain);
		domain = AF_INET;
	}

	if (type > 10) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid socket type [%ld] specified for argument 2, assuming SOCK_STREAM", type);
		type = SOCK_STREAM;
	}

	if (socketpair(domain, type, protocol, fds_array) != 0) {
		SOCKETS_G(last_error) = errno;
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to create socket pair [%d]: %s", errno, sockets_strerror(errno TSRMLS_CC));
		efree(jonj_sock[0]);
		efree(jonj_sock[1]);
		RETURN_FALSE;
	}

	zval_dtor(fds_array_zval);
	array_init(fds_array_zval);

	MAKE_STD_ZVAL(retval[0]);
	MAKE_STD_ZVAL(retval[1]);

	jonj_sock[0]->bsd_socket = fds_array[0];
	jonj_sock[1]->bsd_socket = fds_array[1];
	jonj_sock[0]->type		= domain;
	jonj_sock[1]->type		= domain;
	jonj_sock[0]->error		= 0;
	jonj_sock[1]->error		= 0;
	jonj_sock[0]->blocking	= 1;
	jonj_sock[1]->blocking	= 1;

	ZEND_REGISTER_RESOURCE(retval[0], jonj_sock[0], le_socket);
	ZEND_REGISTER_RESOURCE(retval[1], jonj_sock[1], le_socket);

	add_index_zval(fds_array_zval, 0, retval[0]);
	add_index_zval(fds_array_zval, 1, retval[1]);

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_SHUTDOWN
/* {{{ proto bool socket_shutdown(resource socket[, int how]) U
   Shuts down a socket for receiving, sending, or both. */
JONJ_FUNCTION(socket_shutdown)
{
	zval		*arg1;
	long		how_shutdown = 2;
	jonj_socket	*jonj_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &arg1, &how_shutdown) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket*, &arg1, -1, le_socket_name, le_socket);

	if (shutdown(jonj_sock->bsd_socket, how_shutdown) != 0) {
		JONJ_SOCKET_ERROR(jonj_sock, "unable to shutdown socket", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int socket_last_error([resource socket]) U
   Returns the last socket error (either the last used or the provided socket resource) */
JONJ_FUNCTION(socket_last_error)
{
	zval		*arg1 = NULL;
	jonj_socket	*jonj_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &arg1) == FAILURE) {
		return;
	}

	if (arg1) {
		ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket*, &arg1, -1, le_socket_name, le_socket);
		RETVAL_LONG(jonj_sock->error);
	} else {
		RETVAL_LONG(SOCKETS_G(last_error));
	}
}
/* }}} */

/* {{{ proto void socket_clear_error([resource socket]) U
   Clears the error on the socket or the last error code. */
JONJ_FUNCTION(socket_clear_error)
{
	zval		*arg1 = NULL;
	jonj_socket	*jonj_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r", &arg1) == FAILURE) {
		return;
	}

	if (arg1) {
		ZEND_FETCH_RESOURCE(jonj_sock, jonj_socket*, &arg1, -1, le_socket_name, le_socket);
		jonj_sock->error = 0;
	} else {
		SOCKETS_G(last_error) = 0;
	}

	return;
}
/* }}} */

jonj_socket *socket_import_file_descriptor(JONJ_SOCKET socket TSRMLS_DC)
{
#ifdef SO_DOMAIN
	int						type;
	socklen_t				type_len = sizeof(type);
#endif
	jonj_socket 				*retsock;
	jonj_sockaddr_storage	addr;
	socklen_t				addr_len = sizeof(addr);
#ifndef JONJ_WIN32
	int					 t;
#endif

    retsock = jonj_create_socket();
    retsock->bsd_socket = socket;

    /* determine family */
#ifdef SO_DOMAIN
    if (getsockopt(socket, SOL_SOCKET, SO_DOMAIN, &type, &type_len) == 0) {
		retsock->type = type;
	} else
#endif
	if (getsockname(socket, (struct sockaddr*)&addr, &addr_len) == 0) {
		retsock->type = addr.ss_family;
	} else {
		JONJ_SOCKET_ERROR(retsock, "unable to obtain socket family", errno);
		goto error;
	}

    /* determine blocking mode */
#ifndef JONJ_WIN32
    t = fcntl(socket, F_GETFL);
    if (t == -1) {
		JONJ_SOCKET_ERROR(retsock, "unable to obtain blocking state", errno);
		goto error;
    } else {
    	retsock->blocking = !(t & O_NONBLOCK);
    }
#endif

    return retsock;

error:
	efree(retsock);
	return NULL;
}

/* {{{ proto void socket_import_stream(resource stream)
   Imports a stream that encapsulates a socket into a socket extension resource. */
JONJ_FUNCTION(socket_import_stream)
{
	zval				 *zstream;
	jonj_stream			 *stream;
	jonj_socket			 *retsock = NULL;
	JONJ_SOCKET			 socket; /* fd */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zstream) == FAILURE) {
		return;
	}
	jonj_stream_from_zval(stream, &zstream);

	if (jonj_stream_cast(stream, JONJ_STREAM_AS_SOCKETD, (void**)&socket, 1)) {
		/* error supposedly already shown */
		RETURN_FALSE;
	}

	retsock = socket_import_file_descriptor(socket TSRMLS_CC);
	if (retsock == NULL) {
		RETURN_FALSE;
	}

#ifdef JONJ_WIN32
	/* on windows, check if the stream is a socket stream and read its
	 * private data; otherwise assume it's in non-blocking mode */
	if (jonj_stream_is(stream, JONJ_STREAM_IS_SOCKET)) {
		retsock->blocking =
				((jonj.tkstream_data_t *)stream->abstract)->is_blocked;
	} else {
		retsock->blocking = 1;
	}
#endif

	/* hold a zval reference to the stream (holding a jonj_stream* directly could
	 * also be done, but this might be slightly better if in the future we want
	 * to provide a socket_export_stream) */
	MAKE_STD_ZVAL(retsock->zstream);
	*retsock->zstream = *zstream;
	zval_copy_ctor(retsock->zstream);
	Z_UNSET_ISREF_P(retsock->zstream);
	Z_SET_REFCOUNT_P(retsock->zstream, 1);

	jonj_stream_set_option(stream, JONJ_STREAM_OPTION_READ_BUFFER,
		JONJ_STREAM_BUFFER_NONE, NULL);

	ZEND_REGISTER_RESOURCE(return_value, retsock, le_socket);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
