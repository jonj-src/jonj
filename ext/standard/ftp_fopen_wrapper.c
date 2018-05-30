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
   |          Sara Golemon <pollita@jonj.tk>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "jonj.h"
#include "jonj_globals.h"
#include "jonj.tkwork.h"
#include "jonj_ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef JONJ_WIN32
#include <winsock2.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
#endif

#include "jonj_standard.h"

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef JONJ_WIN32
#include <winsock2.h>
#elif defined(NETWARE) && defined(USE_WINSOCK)
#include <novsock2.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#if defined(JONJ_WIN32) || defined(__riscos__) || defined(NETWARE)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "jonj_fopen_wrappers.h"

#define FTPS_ENCRYPT_DATA 1
#define GET_FTP_RESULT(stream)	get_ftp_result((stream), tmp_line, sizeof(tmp_line) TSRMLS_CC)

typedef struct _jonj_ftp_dirstream_data {
	jonj_stream *datastream;
	jonj_stream *controlstream;
	jonj_stream *dirstream;
} jonj_ftp_dirstream_data;

/* {{{ get_ftp_result
 */
static inline int get_ftp_result(jonj_stream *stream, char *buffer, size_t buffer_size TSRMLS_DC)
{
	buffer[0] = '\0'; /* in case read fails to read anything */
	while (jonj_stream_gets(stream, buffer, buffer_size-1) &&
		   !(isdigit((int) buffer[0]) && isdigit((int) buffer[1]) &&
			 isdigit((int) buffer[2]) && buffer[3] == ' '));
	return strtol(buffer, NULL, 10);
}
/* }}} */

/* {{{ jonj_stream_ftp_stream_stat
 */
static int jonj_stream_ftp_stream_stat(jonj_stream_wrapper *wrapper, jonj_stream *stream, jonj_stream_statbuf *ssb TSRMLS_DC)
{
	/* For now, we return with a failure code to prevent the underlying
	 * file's details from being used instead. */
	return -1;
}
/* }}} */

/* {{{ jonj_stream_ftp_stream_close
 */
static int jonj_stream_ftp_stream_close(jonj_stream_wrapper *wrapper, jonj_stream *stream TSRMLS_DC)
{
	jonj_stream *controlstream = stream->wrapperthis;
	int ret = 0;

	if (controlstream) {
		if (strpbrk(stream->mode, "wa+")) {
			char tmp_line[512];
			int result;

			/* For write modes close data stream first to signal EOF to server */
			result = GET_FTP_RESULT(controlstream);
			if (result != 226 && result != 250) {
				jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "FTP server error %d:%s", result, tmp_line);
				ret = EOF;
			}
		}

		jonj_stream_write_string(controlstream, "QUIT\r\n");
		jonj_stream_close(controlstream);
		stream->wrapperthis = NULL;
	}

	return ret;
}
/* }}} */

/* {{{ jonj_ftp_fopen_connect
 */
static jonj_stream *jonj_ftp_fopen_connect(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options,
										 char **opened_path, jonj_stream_context *context, jonj_stream **preuseid,
										 jonj_url **presource, int *puse_ssl, int *puse_ssl_on_data TSRMLS_DC)
{
	jonj_stream *stream = NULL, *reuseid = NULL;
	jonj_url *resource = NULL;
	int result, use_ssl, use_ssl_on_data = 0, tmp_len;
	char tmp_line[512];
	char *transport;
	int transport_len;

	resource = jonj_url_parse(path);
	if (resource == NULL || resource->path == NULL) {
		if (resource && presource) {
			*presource = resource;
		}
		return NULL;
	}

	use_ssl = resource->scheme && (strlen(resource->scheme) > 3) && resource->scheme[3] == 's';

	/* use port 21 if one wasn't specified */
	if (resource->port == 0)
		resource->port = 21;

	transport_len = spprintf(&transport, 0, "tcp://%s:%d", resource->host, resource->port);
	stream = jonj_stream_xport_create(transport, transport_len, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, NULL, context, NULL, NULL);
	efree(transport);
	if (stream == NULL) {
		result = 0; /* silence */
		goto connect_errexit;
	}

	jonj_stream_context_set(stream, context);
	jonj_stream_notify_info(context, JONJ_STREAM_NOTIFY_CONNECT, NULL, 0);

	/* Start talking to ftp server */
	result = GET_FTP_RESULT(stream);
	if (result > 299 || result < 200) {
		jonj_stream_notify_error(context, JONJ_STREAM_NOTIFY_FAILURE, tmp_line, result);
		goto connect_errexit;
	}

	if (use_ssl)	{

		/* send the AUTH TLS request name */
		jonj_stream_write_string(stream, "AUTH TLS\r\n");

		/* get the response */
		result = GET_FTP_RESULT(stream);
		if (result != 234) {
			/* AUTH TLS not supported try AUTH SSL */
			jonj_stream_write_string(stream, "AUTH SSL\r\n");

			/* get the response */
			result = GET_FTP_RESULT(stream);
			if (result != 334) {
				jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Server doesn't support FTPS.");
				goto connect_errexit;
			} else {
				/* we must reuse the old SSL session id */
				/* if we talk to an old ftpd-ssl */
				reuseid = stream;
			}
		} else {
			/* encrypt data etc */


		}

	}

	if (use_ssl) {
		if (jonj_stream_xport_crypto_setup(stream,
				STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0
				|| jonj_stream_xport_crypto_enable(stream, 1 TSRMLS_CC) < 0) {
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to activate SSL mode");
			jonj_stream_close(stream);
			stream = NULL;
			goto connect_errexit;
		}

		/* set PBSZ to 0 */
		jonj_stream_write_string(stream, "PBSZ 0\r\n");

		/* ignore the response */
		result = GET_FTP_RESULT(stream);

		/* set data connection protection level */
#if FTPS_ENCRYPT_DATA
		jonj_stream_write_string(stream, "PROT P\r\n");

		/* get the response */
		result = GET_FTP_RESULT(stream);
		use_ssl_on_data = (result >= 200 && result<=299) || reuseid;
#else
		jonj_stream_write_string(stream, "PROT C\r\n");

		/* get the response */
		result = GET_FTP_RESULT(stream);
#endif
	}

#define JONJ_FTP_CNTRL_CHK(val, val_len, err_msg) {	\
	unsigned char *s = val, *e = s + val_len;	\
	while (s < e) {	\
		if (iscntrl(*s)) {	\
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, err_msg, val);	\
			goto connect_errexit;	\
		}	\
		s++;	\
	}	\
}

	/* send the user name */
	if (resource->user != NULL) {
		tmp_len = jonj_raw_url_decode(resource->user, strlen(resource->user));

		JONJ_FTP_CNTRL_CHK(resource->user, tmp_len, "Invalid login %s")

		jonj_stream_printf(stream TSRMLS_CC, "USER %s\r\n", resource->user);
	} else {
		jonj_stream_write_string(stream, "USER anonymous\r\n");
	}

	/* get the response */
	result = GET_FTP_RESULT(stream);

	/* if a password is required, send it */
	if (result >= 300 && result <= 399) {
		jonj_stream_notify_info(context, JONJ_STREAM_NOTIFY_AUTH_REQUIRED, tmp_line, 0);

		if (resource->pass != NULL) {
			tmp_len = jonj_raw_url_decode(resource->pass, strlen(resource->pass));

			JONJ_FTP_CNTRL_CHK(resource->pass, tmp_len, "Invalid password %s")

			jonj_stream_printf(stream TSRMLS_CC, "PASS %s\r\n", resource->pass);
		} else {
			/* if the user has configured who they are,
			   send that as the password */
			if (FG(from_address)) {
				jonj_stream_printf(stream TSRMLS_CC, "PASS %s\r\n", FG(from_address));
			} else {
				jonj_stream_write_string(stream, "PASS anonymous\r\n");
			}
		}

		/* read the response */
		result = GET_FTP_RESULT(stream);

		if (result > 299 || result < 200) {
			jonj_stream_notify_error(context, JONJ_STREAM_NOTIFY_AUTH_RESULT, tmp_line, result);
		} else {
			jonj_stream_notify_info(context, JONJ_STREAM_NOTIFY_AUTH_RESULT, tmp_line, result);
		}
	}
	if (result > 299 || result < 200) {
		goto connect_errexit;
	}

	if (puse_ssl) {
		*puse_ssl = use_ssl;
	}
	if (puse_ssl_on_data) {
		*puse_ssl_on_data = use_ssl_on_data;
	}
	if (preuseid) {
		*preuseid = reuseid;
	}
	if (presource) {
		*presource = resource;
	}

	return stream;

connect_errexit:
	if (resource) {
		jonj_url_free(resource);
	}

	if (stream) {
		jonj_stream_close(stream);
	}

	return NULL;
}
/* }}} */

/* {{{ jonj_fopen_do_pasv
 */
static unsigned short jonj_fopen_do_pasv(jonj_stream *stream, char *ip, size_t ip_size, char **phoststart TSRMLS_DC)
{
	char tmp_line[512];
	int result, i;
	unsigned short portno;
	char *tpath, *ttpath, *hoststart=NULL;

#ifdef HAVE_IPV6
	/* We try EPSV first, needed for IPv6 and works on some IPv4 servers */
	jonj_stream_write_string(stream, "EPSV\r\n");
	result = GET_FTP_RESULT(stream);

	/* check if we got a 229 response */
	if (result != 229) {
#endif
		/* EPSV failed, let's try PASV */
		jonj_stream_write_string(stream, "PASV\r\n");
		result = GET_FTP_RESULT(stream);

		/* make sure we got a 227 response */
		if (result != 227) {
			return 0;
		}

		/* parse pasv command (129, 80, 95, 25, 13, 221) */
		tpath = tmp_line;
		/* skip over the "227 Some message " part */
		for (tpath += 4; *tpath && !isdigit((int) *tpath); tpath++);
		if (!*tpath) {
			return 0;
		}
		/* skip over the host ip, to get the port */
		hoststart = tpath;
		for (i = 0; i < 4; i++) {
			for (; isdigit((int) *tpath); tpath++);
			if (*tpath != ',') {
				return 0;
			}
			*tpath='.';
			tpath++;
		}
		tpath[-1] = '\0';
		memcpy(ip, hoststart, ip_size);
		ip[ip_size-1] = '\0';
		hoststart = ip;

		/* pull out the MSB of the port */
		portno = (unsigned short) strtoul(tpath, &ttpath, 10) * 256;
		if (ttpath == NULL) {
			/* didn't get correct response from PASV */
			return 0;
		}
		tpath = ttpath;
		if (*tpath != ',') {
			return 0;
		}
		tpath++;
		/* pull out the LSB of the port */
		portno += (unsigned short) strtoul(tpath, &ttpath, 10);
#ifdef HAVE_IPV6
	} else {
		/* parse epsv command (|||6446|) */
		for (i = 0, tpath = tmp_line + 4; *tpath; tpath++) {
			if (*tpath == '|') {
				i++;
				if (i == 3)
					break;
			}
		}
		if (i < 3) {
			return 0;
		}
		/* pull out the port */
		portno = (unsigned short) strtoul(tpath + 1, &ttpath, 10);
	}
#endif
	if (ttpath == NULL) {
		/* didn't get correct response from EPSV/PASV */
		return 0;
	}

	if (phoststart) {
		*phoststart = hoststart;
	}

	return portno;
}
/* }}} */

/* {{{ jonj_fopen_url_wrap_ftp
 */
jonj_stream * jonj_stream_url_wrap_ftp(jonj_stream_wrapper *wrapper, const char *path, const char *mode,
									 int options, char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC)
{
	jonj_stream *stream = NULL, *datastream = NULL;
	jonj_url *resource = NULL;
	char tmp_line[512];
	char ip[sizeof("123.123.123.123")];
	unsigned short portno;
	char *hoststart = NULL;
	int result = 0, use_ssl, use_ssl_on_data=0;
	jonj_stream *reuseid=NULL;
	size_t file_size = 0;
	zval **tmpzval;
	int allow_overwrite = 0;
	int read_write = 0;
	char *transport;
	int transport_len;

	tmp_line[0] = '\0';

	if (strpbrk(mode, "r+")) {
		read_write = 1; /* Open for reading */
	}
	if (strpbrk(mode, "wa+")) {
		if (read_write) {
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP does not support simultaneous read/write connections");
			return NULL;
		}
		if (strchr(mode, 'a')) {
			read_write = 3; /* Open for Appending */
		} else {
			read_write = 2; /* Open for writing */
		}
	}
	if (!read_write) {
		/* No mode specified? */
		jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unknown file open mode");
		return NULL;
	}

	if (context &&
		jonj_stream_context_get_option(context, "ftp", "proxy", &tmpzval) == SUCCESS) {
		if (read_write == 1) {
			/* Use http wrapper to proxy ftp request */
			return jonj_stream_url_wrap_http(wrapper, path, mode, options, opened_path, context STREAMS_CC TSRMLS_CC);
		} else {
			/* ftp proxy is read-only */
			jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP proxy may only be used in read mode");
			return NULL;
		}
	}

	stream = jonj_ftp_fopen_connect(wrapper, path, mode, options, opened_path, context, &reuseid, &resource, &use_ssl, &use_ssl_on_data TSRMLS_CC);
	if (!stream) {
		goto errexit;
	}

	/* set the connection to be binary */
	jonj_stream_write_string(stream, "TYPE I\r\n");
	result = GET_FTP_RESULT(stream);
	if (result > 299 || result < 200)
		goto errexit;

	/* find out the size of the file (verifying it exists) */
	jonj_stream_printf(stream TSRMLS_CC, "SIZE %s\r\n", resource->path);

	/* read the response */
	result = GET_FTP_RESULT(stream);
	if (read_write == 1) {
		/* Read Mode */
		char *sizestr;

		/* when reading file, it must exist */
		if (result > 299 || result < 200) {
			errno = ENOENT;
			goto errexit;
		}

		sizestr = strchr(tmp_line, ' ');
		if (sizestr) {
			sizestr++;
			file_size = atoi(sizestr);
			jonj_stream_notify_file_size(context, file_size, tmp_line, result);
		}
	} else if (read_write == 2) {
		/* when writing file (but not appending), it must NOT exist, unless a context option exists which allows it */
		if (context && jonj_stream_context_get_option(context, "ftp", "overwrite", &tmpzval) == SUCCESS) {
			allow_overwrite = Z_LVAL_PP(tmpzval);
		}
		if (result <= 299 && result >= 200) {
			if (allow_overwrite) {
				/* Context permits overwriting file,
				   so we just delete whatever's there in preparation */
				jonj_stream_printf(stream TSRMLS_CC, "DELE %s\r\n", resource->path);
				result = GET_FTP_RESULT(stream);
				if (result >= 300 || result <= 199) {
					goto errexit;
				}
			} else {
				jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Remote file already exists and overwrite context option not specified");
				errno = EEXIST;
				goto errexit;
			}
		}
	}

	/* set up the passive connection */
	portno = jonj_fopen_do_pasv(stream, ip, sizeof(ip), &hoststart TSRMLS_CC);

	if (!portno) {
		goto errexit;
	}

	/* Send RETR/STOR command */
	if (read_write == 1) {
		/* set resume position if applicable */
		if (context &&
			jonj_stream_context_get_option(context, "ftp", "resume_pos", &tmpzval) == SUCCESS &&
			Z_TYPE_PP(tmpzval) == IS_LONG &&
			Z_LVAL_PP(tmpzval) > 0) {
			jonj_stream_printf(stream TSRMLS_CC, "REST %ld\r\n", Z_LVAL_PP(tmpzval));
			result = GET_FTP_RESULT(stream);
			if (result < 300 || result > 399) {
				jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to resume from offset %ld", Z_LVAL_PP(tmpzval));
				goto errexit;
			}
		}

		/* retrieve file */
		memcpy(tmp_line, "RETR", sizeof("RETR"));
	} else if (read_write == 2) {
		/* Write new file */
		memcpy(tmp_line, "STOR", sizeof("STOR"));
	} else {
		/* Append */
		memcpy(tmp_line, "APPE", sizeof("APPE"));
	}
	jonj_stream_printf(stream TSRMLS_CC, "%s %s\r\n", tmp_line, (resource->path != NULL ? resource->path : "/"));

	/* open the data channel */
	if (hoststart == NULL) {
		hoststart = resource->host;
	}
	transport_len = spprintf(&transport, 0, "tcp://%s:%d", hoststart, portno);
	datastream = jonj_stream_xport_create(transport, transport_len, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, NULL, NULL, context, NULL, NULL);
	efree(transport);
	if (datastream == NULL) {
		goto errexit;
	}

	result = GET_FTP_RESULT(stream);
	if (result != 150 && result != 125) {
		/* Could not retrieve or send the file
		 * this data will only be sent to us after connection on the data port was initiated.
		 */
		jonj_stream_close(datastream);
		datastream = NULL;
		goto errexit;
	}

	jonj_stream_context_set(datastream, context);
	jonj_stream_notify_progress_init(context, 0, file_size);

	if (use_ssl_on_data && (jonj_stream_xport_crypto_setup(datastream,
			STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0 ||
			jonj_stream_xport_crypto_enable(datastream, 1 TSRMLS_CC) < 0)) {

		jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to activate SSL mode");
		jonj_stream_close(datastream);
		datastream = NULL;
		goto errexit;
	}

	/* remember control stream */
	datastream->wrapperthis = stream;

	jonj_url_free(resource);
	return datastream;

errexit:
	if (resource) {
		jonj_url_free(resource);
	}
	if (stream) {
		jonj_stream_notify_error(context, JONJ_STREAM_NOTIFY_FAILURE, tmp_line, result);
		jonj_stream_close(stream);
	}
	if (tmp_line[0] != '\0')
		jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP server reports %s", tmp_line);
	return NULL;
}
/* }}} */

/* {{{ jonj_ftp_dirsteam_read
 */
static size_t jonj_ftp_dirstream_read(jonj_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	jonj_stream_dirent *ent = (jonj_stream_dirent *)buf;
	jonj_stream *innerstream;
	size_t tmp_len;
	char *basename;
	size_t basename_len;

	innerstream =  ((jonj_ftp_dirstream_data *)stream->abstract)->datastream;

	if (count != sizeof(jonj_stream_dirent)) {
		return 0;
	}

	if (jonj_stream_eof(innerstream)) {
		return 0;
	}

	if (!jonj_stream_get_line(innerstream, ent->d_name, sizeof(ent->d_name), &tmp_len)) {
		return 0;
	}

	jonj_basename(ent->d_name, tmp_len, NULL, 0, &basename, &basename_len TSRMLS_CC);
	if (!basename) {
		return 0;
	}

	if (!basename_len) {
		efree(basename);
		return 0;
	}

	tmp_len = MIN(sizeof(ent->d_name), basename_len - 1);
	memcpy(ent->d_name, basename, tmp_len);
	ent->d_name[tmp_len - 1] = '\0';
	efree(basename);

	/* Trim off trailing whitespace characters */
	while (tmp_len > 0 &&
			(ent->d_name[tmp_len - 1] == '\n' || ent->d_name[tmp_len - 1] == '\r' ||
			 ent->d_name[tmp_len - 1] == '\t' || ent->d_name[tmp_len - 1] == ' ')) {
		ent->d_name[--tmp_len] = '\0';
	}

	return sizeof(jonj_stream_dirent);
}
/* }}} */

/* {{{ jonj_ftp_dirstream_close
 */
static int jonj_ftp_dirstream_close(jonj_stream *stream, int close_handle TSRMLS_DC)
{
	jonj_ftp_dirstream_data *data = stream->abstract;

	/* close control connection */
	if (data->controlstream) {
		jonj_stream_close(data->controlstream);
		data->controlstream = NULL;
	}
	/* close data connection */
	jonj_stream_close(data->datastream);
	data->datastream = NULL;

	efree(data);
	stream->abstract = NULL;

	return 0;
}
/* }}} */

/* ftp dirstreams only need to support read and close operations,
   They can't be rewound because the underlying ftp stream can't be rewound. */
static jonj_stream_ops jonj_ftp_dirstream_ops = {
	NULL, /* write */
	jonj_ftp_dirstream_read, /* read */
	jonj_ftp_dirstream_close, /* close */
	NULL, /* flush */
	"ftpdir",
	NULL, /* rewind */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set option */
};

/* {{{ jonj_stream_ftp_opendir
 */
jonj_stream * jonj_stream_ftp_opendir(jonj_stream_wrapper *wrapper, const char *path, const char *mode, int options,
									char **opened_path, jonj_stream_context *context STREAMS_DC TSRMLS_DC)
{
	jonj_stream *stream, *reuseid, *datastream = NULL;
	jonj_ftp_dirstream_data *dirsdata;
	jonj_url *resource = NULL;
	int result = 0, use_ssl, use_ssl_on_data = 0;
	char *hoststart = NULL, tmp_line[512];
	char ip[sizeof("123.123.123.123")];
	unsigned short portno;

	tmp_line[0] = '\0';

	stream = jonj_ftp_fopen_connect(wrapper, path, mode, options, opened_path, context, &reuseid, &resource, &use_ssl, &use_ssl_on_data TSRMLS_CC);
	if (!stream) {
		goto opendir_errexit;
	}

	/* set the connection to be ascii */
	jonj_stream_write_string(stream, "TYPE A\r\n");
	result = GET_FTP_RESULT(stream);
	if (result > 299 || result < 200)
		goto opendir_errexit;

	// tmp_line isn't relevant after the jonj_fopen_do_pasv().
	tmp_line[0] = '\0';

	/* set up the passive connection */
	portno = jonj_fopen_do_pasv(stream, ip, sizeof(ip), &hoststart TSRMLS_CC);

	if (!portno) {
		goto opendir_errexit;
	}

	/* open the data channel */
	if (hoststart == NULL) {
		hoststart = resource->host;
	}

	datastream = jonj_stream_sock_open_host(hoststart, portno, SOCK_STREAM, 0, 0);
	if (datastream == NULL) {
		goto opendir_errexit;
	}


	jonj_stream_printf(stream TSRMLS_CC, "NLST %s\r\n", (resource->path != NULL ? resource->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result != 150 && result != 125) {
		/* Could not retrieve or send the file
		 * this data will only be sent to us after connection on the data port was initiated.
		 */
		jonj_stream_close(datastream);
		datastream = NULL;
		goto opendir_errexit;
	}

	jonj_stream_context_set(datastream, context);
	if (use_ssl_on_data && (jonj_stream_xport_crypto_setup(datastream,
			STREAM_CRYPTO_METHOD_SSLv23_CLIENT, NULL TSRMLS_CC) < 0 ||
			jonj_stream_xport_crypto_enable(datastream, 1 TSRMLS_CC) < 0)) {

		jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "Unable to activate SSL mode");
		jonj_stream_close(datastream);
		datastream = NULL;
		goto opendir_errexit;
	}

	jonj_url_free(resource);

	dirsdata = emalloc(sizeof *dirsdata);
	dirsdata->datastream = datastream;
	dirsdata->controlstream = stream;
	dirsdata->dirstream = jonj_stream_alloc(&jonj_ftp_dirstream_ops, dirsdata, 0, mode);

	return dirsdata->dirstream;

opendir_errexit:
	if (resource) {
		jonj_url_free(resource);
	}
	if (stream) {
		jonj_stream_notify_error(context, JONJ_STREAM_NOTIFY_FAILURE, tmp_line, result);
		jonj_stream_close(stream);
	}
	if (tmp_line[0] != '\0') {
		jonj_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "FTP server reports %s", tmp_line);
	}
	return NULL;
}
/* }}} */

/* {{{ jonj_stream_ftp_url_stat
 */
static int jonj_stream_ftp_url_stat(jonj_stream_wrapper *wrapper, const char *url, int flags, jonj_stream_statbuf *ssb, jonj_stream_context *context TSRMLS_DC)
{
	jonj_stream *stream = NULL;
	jonj_url *resource = NULL;
	int result;
	char tmp_line[512];

	/* If ssb is NULL then someone is misbehaving */
	if (!ssb) return -1;

	stream = jonj_ftp_fopen_connect(wrapper, url, "r", 0, NULL, context, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		goto stat_errexit;
	}

	ssb->sb.st_mode = 0644;									/* FTP won't give us a valid mode, so aproximate one based on being readable */
	jonj_stream_printf(stream TSRMLS_CC, "CWD %s\r\n", (resource->path != NULL ? resource->path : "/")); /* If we can CWD to it, it's a directory (maybe a link, but we can't tell) */
	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		ssb->sb.st_mode |= S_IFREG;
	} else {
		ssb->sb.st_mode |= S_IFDIR;
	}

	jonj_stream_write_string(stream, "TYPE I\r\n"); /* we need this since some servers refuse to accept SIZE command in ASCII mode */

	result = GET_FTP_RESULT(stream);

	if(result < 200 || result > 299) {
		goto stat_errexit;
	}

	jonj_stream_printf(stream TSRMLS_CC, "SIZE %s\r\n", (resource->path != NULL ? resource->path : "/"));
	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		/* Failure either means it doesn't exist
		   or it's a directory and this server
		   fails on listing directory sizes */
		if (ssb->sb.st_mode & S_IFDIR) {
			ssb->sb.st_size = 0;
		} else {
			goto stat_errexit;
		}
	} else {
		ssb->sb.st_size = atoi(tmp_line + 4);
	}

	jonj_stream_printf(stream TSRMLS_CC, "MDTM %s\r\n", (resource->path != NULL ? resource->path : "/"));
	result = GET_FTP_RESULT(stream);
	if (result == 213) {
		char *p = tmp_line + 4;
		int n;
		struct tm tm, tmbuf, *gmt;
		time_t stamp;

		while (p - tmp_line < sizeof(tmp_line) && !isdigit(*p)) {
			p++;
		}

		if (p - tmp_line > sizeof(tmp_line)) {
			goto mdtm_error;
		}

		n = sscanf(p, "%4u%2u%2u%2u%2u%2u", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
		if (n != 6) {
			goto mdtm_error;
		}

		tm.tm_year -= 1900;
		tm.tm_mon--;
		tm.tm_isdst = -1;

		/* figure out the GMT offset */
		stamp = time(NULL);
		gmt = jonj_gmtime_r(&stamp, &tmbuf);
		if (!gmt) {
			goto mdtm_error;
		}
		gmt->tm_isdst = -1;

		/* apply the GMT offset */
		tm.tm_sec += stamp - mktime(gmt);
		tm.tm_isdst = gmt->tm_isdst;

		ssb->sb.st_mtime = mktime(&tm);
	} else {
		/* error or unsupported command */
mdtm_error:
		ssb->sb.st_mtime = -1;
	}

	ssb->sb.st_ino = 0;						/* Unknown values */
	ssb->sb.st_dev = 0;
	ssb->sb.st_uid = 0;
	ssb->sb.st_gid = 0;
	ssb->sb.st_atime = -1;
	ssb->sb.st_ctime = -1;

	ssb->sb.st_nlink = 1;
	ssb->sb.st_rdev = -1;
#ifdef HAVE_ST_BLKSIZE
	ssb->sb.st_blksize = 4096;				/* Guess since FTP won't expose this information */
#ifdef HAVE_ST_BLOCKS
	ssb->sb.st_blocks = (int)((4095 + ssb->sb.st_size) / ssb->sb.st_blksize); /* emulate ceil */
#endif
#endif
	jonj_stream_close(stream);
	jonj_url_free(resource);
	return 0;

stat_errexit:
	if (resource) {
		jonj_url_free(resource);
	}
	if (stream) {
		jonj_stream_close(stream);
	}
	return -1;
}
/* }}} */

/* {{{ jonj_stream_ftp_unlink
 */
static int jonj_stream_ftp_unlink(jonj_stream_wrapper *wrapper, const char *url, int options, jonj_stream_context *context TSRMLS_DC)
{
	jonj_stream *stream = NULL;
	jonj_url *resource = NULL;
	int result;
	char tmp_line[512];

	stream = jonj_ftp_fopen_connect(wrapper, url, "r", 0, NULL, NULL, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", url);
		}
		goto unlink_errexit;
	}

	if (resource->path == NULL) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path provided in %s", url);
		}
		goto unlink_errexit;
	}

	/* Attempt to delete the file */
	jonj_stream_printf(stream TSRMLS_CC, "DELE %s\r\n", (resource->path != NULL ? resource->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Error Deleting file: %s", tmp_line);
		}
		goto unlink_errexit;
	}

	jonj_url_free(resource);
	jonj_stream_close(stream);
	return 1;

unlink_errexit:
	if (resource) {
		jonj_url_free(resource);
	}
	if (stream) {
		jonj_stream_close(stream);
	}
	return 0;
}
/* }}} */

/* {{{ jonj_stream_ftp_rename
 */
static int jonj_stream_ftp_rename(jonj_stream_wrapper *wrapper, const char *url_from, const char *url_to, int options, jonj_stream_context *context TSRMLS_DC)
{
	jonj_stream *stream = NULL;
	jonj_url *resource_from = NULL, *resource_to = NULL;
	int result;
	char tmp_line[512];

	resource_from = jonj_url_parse(url_from);
	resource_to = jonj_url_parse(url_to);
	/* Must be same scheme (ftp/ftp or ftps/ftps), same host, and same port
		(or a 21/0 0/21 combination which is also "same")
	   Also require paths to/from */
	if (!resource_from ||
		!resource_to ||
		!resource_from->scheme ||
		!resource_to->scheme ||
		strcmp(resource_from->scheme, resource_to->scheme) ||
		!resource_from->host ||
		!resource_to->host ||
		strcmp(resource_from->host, resource_to->host) ||
		(resource_from->port != resource_to->port &&
		 resource_from->port * resource_to->port != 0 &&
		 resource_from->port + resource_to->port != 21) ||
		!resource_from->path ||
		!resource_to->path) {
		goto rename_errexit;
	}

	stream = jonj_ftp_fopen_connect(wrapper, url_from, "r", 0, NULL, NULL, NULL, NULL, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", resource_from->host);
		}
		goto rename_errexit;
	}

	/* Rename FROM */
	jonj_stream_printf(stream TSRMLS_CC, "RNFR %s\r\n", (resource_from->path != NULL ? resource_from->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result < 300 || result > 399) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Error Renaming file: %s", tmp_line);
		}
		goto rename_errexit;
	}

	/* Rename TO */
	jonj_stream_printf(stream TSRMLS_CC, "RNTO %s\r\n", (resource_to->path != NULL ? resource_to->path : "/"));

	result = GET_FTP_RESULT(stream);
	if (result < 200 || result > 299) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Error Renaming file: %s", tmp_line);
		}
		goto rename_errexit;
	}

	jonj_url_free(resource_from);
	jonj_url_free(resource_to);
	jonj_stream_close(stream);
	return 1;

rename_errexit:
	if (resource_from) {
		jonj_url_free(resource_from);
	}
	if (resource_to) {
		jonj_url_free(resource_to);
	}
	if (stream) {
		jonj_stream_close(stream);
	}
	return 0;
}
/* }}} */

/* {{{ jonj_stream_ftp_mkdir
 */
static int jonj_stream_ftp_mkdir(jonj_stream_wrapper *wrapper, const char *url, int mode, int options, jonj_stream_context *context TSRMLS_DC)
{
	jonj_stream *stream = NULL;
	jonj_url *resource = NULL;
	int result, recursive = options & JONJ_STREAM_MKDIR_RECURSIVE;
	char tmp_line[512];

	stream = jonj_ftp_fopen_connect(wrapper, url, "r", 0, NULL, NULL, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", url);
		}
		goto mkdir_errexit;
	}

	if (resource->path == NULL) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path provided in %s", url);
		}
		goto mkdir_errexit;
	}

	if (!recursive) {
		jonj_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", resource->path);
		result = GET_FTP_RESULT(stream);
    } else {
        /* we look for directory separator from the end of string, thus hopefuly reducing our work load */
        char *p, *e, *buf;

        buf = estrdup(resource->path);
        e = buf + strlen(buf);

        /* find a top level directory we need to create */
        while ((p = strrchr(buf, '/'))) {
            *p = '\0';
			jonj_stream_printf(stream TSRMLS_CC, "CWD %s\r\n", buf);
			result = GET_FTP_RESULT(stream);
			if (result >= 200 && result <= 299) {
				*p = '/';
				break;
			}
        }
        if (p == buf) {
			jonj_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", resource->path);
			result = GET_FTP_RESULT(stream);
        } else {
			jonj_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", buf);
			result = GET_FTP_RESULT(stream);
			if (result >= 200 && result <= 299) {
				if (!p) {
					p = buf;
				}
				/* create any needed directories if the creation of the 1st directory worked */
				while (++p != e) {
					if (*p == '\0' && *(p + 1) != '\0') {
						*p = '/';
						jonj_stream_printf(stream TSRMLS_CC, "MKD %s\r\n", buf);
						result = GET_FTP_RESULT(stream);
						if (result < 200 || result > 299) {
							if (options & REPORT_ERRORS) {
								jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", tmp_line);
							}
							break;
						}
					}
				}
			}
		}
        efree(buf);
    }

	jonj_url_free(resource);
	jonj_stream_close(stream);

	if (result < 200 || result > 299) {
		/* Failure */
		return 0;
	}

	return 1;

mkdir_errexit:
	if (resource) {
		jonj_url_free(resource);
	}
	if (stream) {
		jonj_stream_close(stream);
	}
	return 0;
}
/* }}} */

/* {{{ jonj_stream_ftp_rmdir
 */
static int jonj_stream_ftp_rmdir(jonj_stream_wrapper *wrapper, const char *url, int options, jonj_stream_context *context TSRMLS_DC)
{
	jonj_stream *stream = NULL;
	jonj_url *resource = NULL;
	int result;
	char tmp_line[512];

	stream = jonj_ftp_fopen_connect(wrapper, url, "r", 0, NULL, NULL, NULL, &resource, NULL, NULL TSRMLS_CC);
	if (!stream) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to connect to %s", url);
		}
		goto rmdir_errexit;
	}

	if (resource->path == NULL) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid path provided in %s", url);
		}
		goto rmdir_errexit;
	}

	jonj_stream_printf(stream TSRMLS_CC, "RMD %s\r\n", resource->path);
	result = GET_FTP_RESULT(stream);

	if (result < 200 || result > 299) {
		if (options & REPORT_ERRORS) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", tmp_line);
		}
		goto rmdir_errexit;
	}

	jonj_url_free(resource);
	jonj_stream_close(stream);

	return 1;

rmdir_errexit:
	if (resource) {
		jonj_url_free(resource);
	}
	if (stream) {
		jonj_stream_close(stream);
	}
	return 0;
}
/* }}} */

static jonj_stream_wrapper_ops ftp_stream_wops = {
	jonj_stream_url_wrap_ftp,
	jonj_stream_ftp_stream_close, /* stream_close */
	jonj_stream_ftp_stream_stat,
	jonj_stream_ftp_url_stat, /* stat_url */
	jonj_stream_ftp_opendir, /* opendir */
	"ftp",
	jonj_stream_ftp_unlink, /* unlink */
	jonj_stream_ftp_rename, /* rename */
	jonj_stream_ftp_mkdir,  /* mkdir */
	jonj_stream_ftp_rmdir   /* rmdir */
};

JONJAPI jonj_stream_wrapper jonj_stream_ftp_wrapper =	{
	&ftp_stream_wops,
	NULL,
	1 /* is_url */
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
