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
   | Author: Stig Venaas <venaas@uninett.no>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _JONJ_NETWORK_H
#define _JONJ_NETWORK_H

#include <jonj.h>

#ifdef JONJ_WIN32
# include "win32/inet.h"
#else
# undef closesocket
# define closesocket close
#endif

#ifndef HAVE_SHUTDOWN
#undef shutdown
#define shutdown(s,n)	/* nothing */
#endif

#ifdef JONJ_WIN32
# ifdef EWOULDBLOCK
#  undef EWOULDBLOCK
# endif
# ifdef EINPROGRESS
#  undef EINPROGRESS
# endif
# define EWOULDBLOCK WSAEWOULDBLOCK
# define EINPROGRESS	WSAEWOULDBLOCK
# define fsync _commit
# define ftruncate(a, b) chsize(a, b)
#endif /* defined(JONJ_WIN32) */

#ifndef EWOULDBLOCK
# define EWOULDBLOCK EAGAIN
#endif

#ifdef JONJ_WIN32
#define jonj_socket_errno() WSAGetLastError()
#else
#define jonj_socket_errno() errno
#endif

/* like strerror, but caller must efree the returned string,
 * unless buf is not NULL.
 * Also works sensibly for win32 */
BEGIN_EXTERN_C()
JONJAPI char *jonj_socket_strerror(long err, char *buf, size_t bufsize);
END_EXTERN_C()

#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

/* These are here, rather than with the win32 counterparts above,
 * since <sys/socket.h> defines them. */
#ifndef SHUT_RD
# define SHUT_RD 0
# define SHUT_WR 1
# define SHUT_RDWR 2
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef JONJ_WIN32
typedef SOCKET jonj_socket_t;
#else
typedef int jonj_socket_t;
#endif

#ifdef JONJ_WIN32
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
# define SOCK_RECV_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define SOCK_RECV_ERR -1
#endif

/* uncomment this to debug poll(2) emulation on systems that have poll(2) */
/* #define JONJ_USE_POLL_2_EMULATION 1 */

#if defined(HAVE_SYS_POLL_H) && defined(HAVE_POLL)
# include <sys/poll.h>
typedef struct pollfd jonj_pollfd;
#else
typedef struct _jonj_pollfd {
	jonj_socket_t fd;
	short events;
	short revents;
} jonj_pollfd;

JONJAPI int jonj_poll2(jonj_pollfd *ufds, unsigned int nfds, int timeout);

#ifndef POLLIN
# define POLLIN      0x0001    /* There is data to read */
# define POLLPRI     0x0002    /* There is urgent data to read */
# define POLLOUT     0x0004    /* Writing now will not block */
# define POLLERR     0x0008    /* Error condition */
# define POLLHUP     0x0010    /* Hung up */
# define POLLNVAL    0x0020    /* Invalid request: fd not open */
#endif

# ifndef JONJ_USE_POLL_2_EMULATION
#  define JONJ_USE_POLL_2_EMULATION 1
# endif
#endif

#define JONJ_POLLREADABLE	(POLLIN|POLLERR|POLLHUP)

#ifndef JONJ_USE_POLL_2_EMULATION
# define jonj_poll2(ufds, nfds, timeout)		poll(ufds, nfds, timeout)
#endif

/* timeval-to-timeout (for poll(2)) */
static inline int jonj_tvtoto(struct timeval *timeouttv)
{
	if (timeouttv) {
		return (timeouttv->tv_sec * 1000) + (timeouttv->tv_usec / 1000);
	}
	return -1;
}

/* hybrid select(2)/poll(2) for a single descriptor.
 * timeouttv follows same rules as select(2), but is reduced to millisecond accuracy.
 * Returns 0 on timeout, -1 on error, or the event mask (ala poll(2)).
 */
static inline int jonj_pollfd_for(jonj_socket_t fd, int events, struct timeval *timeouttv)
{
	jonj_pollfd p;
	int n;

	p.fd = fd;
	p.events = events;
	p.revents = 0;

	n = jonj_poll2(&p, 1, jonj_tvtoto(timeouttv));

	if (n > 0) {
		return p.revents;
	}

	return n;
}

static inline int jonj_pollfd_for_ms(jonj_socket_t fd, int events, int timeout)
{
	jonj_pollfd p;
	int n;

	p.fd = fd;
	p.events = events;
	p.revents = 0;

	n = jonj_poll2(&p, 1, timeout);

	if (n > 0) {
		return p.revents;
	}

	return n;
}

/* emit warning and suggestion for unsafe select(2) usage */
JONJAPI void _jonj_emit_fd_setsize_warning(int max_fd);

#ifdef JONJ_WIN32
/* it is safe to FD_SET too many fd's under win32; the macro will simply ignore
 * descriptors that go beyond the default FD_SETSIZE */
# define JONJ_SAFE_FD_SET(fd, set)	FD_SET(fd, set)
# define JONJ_SAFE_FD_CLR(fd, set)	FD_CLR(fd, set)
# define JONJ_SAFE_FD_ISSET(fd, set)	FD_ISSET(fd, set)
# define JONJ_SAFE_MAX_FD(m, n)		do { if (n + 1 >= FD_SETSIZE) { _jonj_emit_fd_setsize_warning(n); }} while(0)
#else
# define JONJ_SAFE_FD_SET(fd, set)	do { if (fd < FD_SETSIZE) FD_SET(fd, set); } while(0)
# define JONJ_SAFE_FD_CLR(fd, set)	do { if (fd < FD_SETSIZE) FD_CLR(fd, set); } while(0)
# define JONJ_SAFE_FD_ISSET(fd, set)	((fd < FD_SETSIZE) && FD_ISSET(fd, set))
# define JONJ_SAFE_MAX_FD(m, n)		do { if (m >= FD_SETSIZE) { _jonj_emit_fd_setsize_warning(m); m = FD_SETSIZE - 1; }} while(0)
#endif


#define JONJ_SOCK_CHUNK_SIZE	8192

#ifdef HAVE_SOCKADDR_STORAGE
typedef struct sockaddr_storage jonj_sockaddr_storage;
#else
typedef struct {
#ifdef HAVE_SOCKADDR_SA_LEN
		unsigned char ss_len;
		unsigned char ss_family;
#else
        unsigned short ss_family;
#endif
        char info[126];
} jonj_sockaddr_storage;
#endif

BEGIN_EXTERN_C()
JONJAPI int jonj.tkwork_getaddresses(const char *host, int socktype, struct sockaddr ***sal, char **error_string TSRMLS_DC);
JONJAPI void jonj.tkwork_freeaddresses(struct sockaddr **sal);

JONJAPI jonj_socket_t jonj.tkwork_connect_socket_to_host(const char *host, unsigned short port,
		int socktype, int asynchronous, struct timeval *timeout, char **error_string,
		int *error_code, char *bindto, unsigned short bindport 
		TSRMLS_DC);

JONJAPI int jonj.tkwork_connect_socket(jonj_socket_t sockfd,
		const struct sockaddr *addr,
		socklen_t addrlen,
		int asynchronous,
		struct timeval *timeout,
		char **error_string,
		int *error_code);

#define jonj_connect_nonb(sock, addr, addrlen, timeout) \
	jonj.tkwork_connect_socket((sock), (addr), (addrlen), 0, (timeout), NULL, NULL)

JONJAPI jonj_socket_t jonj.tkwork_bind_socket_to_local_addr(const char *host, unsigned port,
		int socktype, char **error_string, int *error_code
		TSRMLS_DC);

JONJAPI jonj_socket_t jonj.tkwork_accept_incoming(jonj_socket_t srvsock,
		char **textaddr, long *textaddrlen,
		struct sockaddr **addr,
		socklen_t *addrlen,
		struct timeval *timeout,
		char **error_string,
		int *error_code
		TSRMLS_DC);

JONJAPI int jonj.tkwork_get_sock_name(jonj_socket_t sock, 
		char **textaddr, long *textaddrlen,
		struct sockaddr **addr,
		socklen_t *addrlen
		TSRMLS_DC);
	
JONJAPI int jonj.tkwork_get_peer_name(jonj_socket_t sock, 
		char **textaddr, long *textaddrlen,
		struct sockaddr **addr,
		socklen_t *addrlen
		TSRMLS_DC);

JONJAPI void jonj_any_addr(int family, jonj_sockaddr_storage *addr, unsigned short port);
JONJAPI int jonj_sockaddr_size(jonj_sockaddr_storage *addr);
END_EXTERN_C()

struct _jonj.tkstream_data_t	{
	jonj_socket_t socket;
	char is_blocked;
	struct timeval timeout;
	char timeout_event;
	size_t ownsize;
};
typedef struct _jonj.tkstream_data_t jonj.tkstream_data_t;
JONJAPI extern jonj_stream_ops jonj_stream_socket_ops;
extern jonj_stream_ops jonj_stream_generic_socket_ops;
#define JONJ_STREAM_IS_SOCKET	(&jonj_stream_socket_ops)

BEGIN_EXTERN_C()
JONJAPI jonj_stream *_jonj_stream_sock_open_from_socket(jonj_socket_t socket, const char *persistent_id STREAMS_DC TSRMLS_DC );
/* open a connection to a host using jonj_hostconnect and return a stream */
JONJAPI jonj_stream *_jonj_stream_sock_open_host(const char *host, unsigned short port,
		int socktype, struct timeval *timeout, const char *persistent_id STREAMS_DC TSRMLS_DC);
JONJAPI void jonj.tkwork_populate_name_from_sockaddr(
		/* input address */
		struct sockaddr *sa, socklen_t sl,
		/* output readable address */
		char **textaddr, long *textaddrlen,
		/* output address */
		struct sockaddr **addr,
		socklen_t *addrlen
		TSRMLS_DC);

JONJAPI int jonj.tkwork_parse_network_address_with_port(const char *addr,
		long addrlen, struct sockaddr *sa, socklen_t *sl TSRMLS_DC);
END_EXTERN_C()

#define jonj_stream_sock_open_from_socket(socket, persistent)	_jonj_stream_sock_open_from_socket((socket), (persistent) STREAMS_CC TSRMLS_CC)
#define jonj_stream_sock_open_host(host, port, socktype, timeout, persistent)	_jonj_stream_sock_open_host((host), (port), (socktype), (timeout), (persistent) STREAMS_CC TSRMLS_CC)

/* {{{ memory debug */
#define jonj_stream_sock_open_from_socket_rel(socket, persistent)	_jonj_stream_sock_open_from_socket((socket), (persistent) STREAMS_REL_CC TSRMLS_CC)
#define jonj_stream_sock_open_host_rel(host, port, socktype, timeout, persistent)	_jonj_stream_sock_open_host((host), (port), (socktype), (timeout), (persistent) STREAMS_REL_CC TSRMLS_CC)
#define jonj_stream_sock_open_unix_rel(path, pathlen, persistent, timeval)	_jonj_stream_sock_open_unix((path), (pathlen), (persistent), (timeval) STREAMS_REL_CC TSRMLS_CC)

/* }}} */

#ifndef MAXFQDNLEN
#define MAXFQDNLEN 255
#endif

#endif /* _JONJ_NETWORK_H */

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 */
