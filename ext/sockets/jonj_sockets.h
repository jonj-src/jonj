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
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@jonj4win.de>                      |
   +----------------------------------------------------------------------+
 */

#ifndef JONJ_SOCKETS_H
#define JONJ_SOCKETS_H

/* $Id$ */

#if HAVE_CONFIG_H
# include "config.h"
#endif

#if HAVE_SOCKETS

#include <jonj.h>
#ifdef JONJ_WIN32
# include "windows_common.h"
#endif

extern zend_module_entry sockets_module_entry;
#define jonjext_sockets_ptr &sockets_module_entry

#ifdef JONJ_WIN32
#include <Winsock2.h>
#else
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#endif

#ifndef JONJ_WIN32
typedef int JONJ_SOCKET;
# define JONJ_SOCKETS_API JONJAPI
#else
# define JONJ_SOCKETS_API __declspec(dllexport)
typedef SOCKET JONJ_SOCKET;
#endif

typedef struct {
	JONJ_SOCKET	bsd_socket;
	int			type;
	int			error;
	int			blocking;
	zval		*zstream;
} jonj_socket;

#ifdef JONJ_WIN32
struct	sockaddr_un {
	short	sun_family;
	char	sun_path[108];
};
#endif

JONJ_SOCKETS_API int jonj_sockets_le_socket(void);

#define jonj_sockets_le_socket_name "Socket"

#define JONJ_SOCKET_ERROR(socket, msg, errn) \
		do { \
			int _err = (errn); /* save value to avoid repeated calls to WSAGetLastError() on Windows */ \
			(socket)->error = _err; \
			SOCKETS_G(last_error) = _err; \
			if (_err != EAGAIN && _err != EWOULDBLOCK && _err != EINPROGRESS) { \
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "%s [%d]: %s", msg, _err, sockets_strerror(_err TSRMLS_CC)); \
			} \
		} while (0)

ZEND_BEGIN_MODULE_GLOBALS(sockets)
	int last_error;
	char *strerror_buf;
ZEND_END_MODULE_GLOBALS(sockets)

#ifdef ZTS
#define SOCKETS_G(v) TSRMG(sockets_globals_id, zend_sockets_globals *, v)
#else
#define SOCKETS_G(v) (sockets_globals.v)
#endif

ZEND_EXTERN_MODULE_GLOBALS(sockets);

enum sockopt_return {
	SOCKOPT_ERROR,
	SOCKOPT_CONTINUE,
	SOCKOPT_SUCCESS
};

char *sockets_strerror(int error TSRMLS_DC);
jonj_socket *socket_import_file_descriptor(JONJ_SOCKET sock TSRMLS_DC);

#else
#define jonjext_sockets_ptr NULL
#endif

#if defined(_AIX) && !defined(HAVE_SA_SS_FAMILY)
# define ss_family __ss_family
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */

