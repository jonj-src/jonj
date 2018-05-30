#ifndef JONJ_SOCKADR_CONV_H
#define JONJ_SOCKADR_CONV_H

#include <jonj.tkwork.h>
#include "jonj_sockets.h" /* jonj_socket */

#ifndef JONJ_WIN32
# include <netinet/in.h>
#else
# include <Winsock2.h>
#endif


/*
 * Convert an IPv6 literal or a hostname info a sockaddr_in6.
 * The IPv6 literal can be a IPv4 mapped address (like ::ffff:127.0.0.1).
 * If the hostname yields no IPv6 addresses, a mapped IPv4 address may be returned (AI_V4MAPPED)
 */
int jonj_set_inet6_addr(struct sockaddr_in6 *sin6, char *string, jonj_socket *jonj_sock TSRMLS_DC);

/*
 * Convert an IPv4 literal or a hostname into a sockaddr_in.
 */
int jonj_set_inet_addr(struct sockaddr_in *sin, char *string, jonj_socket *jonj_sock TSRMLS_DC);

/*
 * Calls either jonj_set_inet6_addr() or jonj_set_inet_addr(), depending on the type of the socket.
 */
int jonj_set_inet46_addr(jonj_sockaddr_storage *ss, socklen_t *ss_len, char *string, jonj_socket *jonj_sock TSRMLS_DC);

#endif
