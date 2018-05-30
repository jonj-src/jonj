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
   | Authors: Gustavo Lopes    <cataphract@jonj.tk>                       |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#if defined(MCAST_JOIN_GROUP) && !defined(__APPLE__)
# define RFC3678_API 1
/* has block/unblock and source membership, in this case for both IPv4 and IPv6 */
# define HAS_MCAST_EXT 1
#elif defined(IP_ADD_SOURCE_MEMBERSHIP) && !defined(__APPLE__)
/* has block/unblock and source membership, but only for IPv4 */
# define HAS_MCAST_EXT 1
#endif

#ifndef RFC3678_API
# define JONJ_MCAST_JOIN_GROUP			IP_ADD_MEMBERSHIP
# define JONJ_MCAST_LEAVE_GROUP			IP_DROP_MEMBERSHIP
# ifdef HAS_MCAST_EXT
#  define JONJ_MCAST_BLOCK_SOURCE		IP_BLOCK_SOURCE
#  define JONJ_MCAST_UNBLOCK_SOURCE		IP_UNBLOCK_SOURCE
#  define JONJ_MCAST_JOIN_SOURCE_GROUP	IP_ADD_SOURCE_MEMBERSHIP
#  define JONJ_MCAST_LEAVE_SOURCE_GROUP	IP_DROP_SOURCE_MEMBERSHIP
# endif
#else
# define JONJ_MCAST_JOIN_GROUP			MCAST_JOIN_GROUP
# define JONJ_MCAST_LEAVE_GROUP			MCAST_LEAVE_GROUP
# define JONJ_MCAST_BLOCK_SOURCE			MCAST_BLOCK_SOURCE
# define JONJ_MCAST_UNBLOCK_SOURCE		MCAST_UNBLOCK_SOURCE
# define JONJ_MCAST_JOIN_SOURCE_GROUP	MCAST_JOIN_SOURCE_GROUP
# define JONJ_MCAST_LEAVE_SOURCE_GROUP	MCAST_LEAVE_SOURCE_GROUP
#endif

int jonj_do_setsockopt_ip_mcast(jonj_socket *jonj_sock,
							   int level,
							   int optname,
							   zval **arg4 TSRMLS_DC);

int jonj_do_setsockopt_ipv6_mcast(jonj_socket *jonj_sock,
								 int level,
								 int optname,
								 zval **arg4 TSRMLS_DC);

int jonj_if_index_to_addr4(
        unsigned if_index,
        jonj_socket *jonj_sock,
        struct in_addr *out_addr TSRMLS_DC);

int jonj_add4_to_if_index(
        struct in_addr *addr,
        jonj_socket *jonj_sock,
        unsigned *if_index TSRMLS_DC);

int jonj_string_to_if_index(const char *val, unsigned *out TSRMLS_DC);

int jonj_mcast_join(
	jonj_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	unsigned int if_index TSRMLS_DC);

int jonj_mcast_leave(
	jonj_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	unsigned int if_index TSRMLS_DC);

#ifdef HAS_MCAST_EXT
int jonj_mcast_join_source(
	jonj_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);

int jonj_mcast_leave_source(
	jonj_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);

int jonj_mcast_block_source(
	jonj_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);

int jonj_mcast_unblock_source(
	jonj_socket *sock,
	int level,
	struct sockaddr *group,
	socklen_t group_len,
	struct sockaddr *source,
	socklen_t source_len,
	unsigned int if_index TSRMLS_DC);
#endif
