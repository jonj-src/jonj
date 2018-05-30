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
   | Authors: Marcus Boerger <helly@jonj.tk>                              |
   | based on ext/db/db.c by:                                             |
   |          Rasmus Lerdorf <rasmus@jonj.tk>                             |
   |          Jim Winstead <jimw@jonj.tk>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"
#include "jonj_globals.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "flatfile.h"

#define FLATFILE_BLOCK_SIZE 1024

/*
 * ret = -1 means that database was opened for read-only
 * ret = 0  success
 * ret = 1  key already exists - nothing done
 */

/* {{{ flatfile_store
 */
int flatfile_store(flatfile *dba, datum key_datum, datum value_datum, int mode TSRMLS_DC) {
	if (mode == FLATFILE_INSERT) {
		if (flatfile_findkey(dba, key_datum TSRMLS_CC)) {
			return 1;
		}
		jonj_stream_seek(dba->fp, 0L, SEEK_END);
		jonj_stream_printf(dba->fp TSRMLS_CC, "%zu\n", key_datum.dsize);
		jonj_stream_flush(dba->fp);
		if (jonj_stream_write(dba->fp, key_datum.dptr, key_datum.dsize) < key_datum.dsize) {
			return -1;
		}
		jonj_stream_printf(dba->fp TSRMLS_CC, "%zu\n", value_datum.dsize);
		jonj_stream_flush(dba->fp);
		if (jonj_stream_write(dba->fp, value_datum.dptr, value_datum.dsize) < value_datum.dsize) {
			return -1;
		}
	} else { /* FLATFILE_REPLACE */
		flatfile_delete(dba, key_datum TSRMLS_CC);
		jonj_stream_printf(dba->fp TSRMLS_CC, "%zu\n", key_datum.dsize);
		jonj_stream_flush(dba->fp);
		if (jonj_stream_write(dba->fp, key_datum.dptr, key_datum.dsize) < key_datum.dsize) {
			return -1;
		}
		jonj_stream_printf(dba->fp TSRMLS_CC, "%zu\n", value_datum.dsize);
		if (jonj_stream_write(dba->fp, value_datum.dptr, value_datum.dsize) < value_datum.dsize) {
			return -1;
		}
	}

	jonj_stream_flush(dba->fp);
	return 0;
}
/* }}} */

/* {{{ flatfile_fetch
 */
datum flatfile_fetch(flatfile *dba, datum key_datum TSRMLS_DC) {
	datum value_datum = {NULL, 0};
	char buf[16];

	if (flatfile_findkey(dba, key_datum TSRMLS_CC)) {
		if (jonj_stream_gets(dba->fp, buf, sizeof(buf))) {
			value_datum.dsize = atoi(buf);
			value_datum.dptr = safe_emalloc(value_datum.dsize, 1, 1);
			value_datum.dsize = jonj_stream_read(dba->fp, value_datum.dptr, value_datum.dsize);
		} else {
			value_datum.dptr = NULL;
			value_datum.dsize = 0;
		}
	}
	return value_datum;
}
/* }}} */

/* {{{ flatfile_delete
 */
int flatfile_delete(flatfile *dba, datum key_datum TSRMLS_DC) {
	char *key = key_datum.dptr;
	size_t size = key_datum.dsize;
	size_t buf_size = FLATFILE_BLOCK_SIZE;
	char *buf = emalloc(buf_size);
	size_t num;
	size_t pos;

	jonj_stream_rewind(dba->fp);
	while(!jonj_stream_eof(dba->fp)) {
		/* read in the length of the key name */
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		pos = jonj_stream_tell(dba->fp);

		/* read in the key name */
		num = jonj_stream_read(dba->fp, buf, num);

		if (size == num && !memcmp(buf, key, size)) {
			jonj_stream_seek(dba->fp, pos, SEEK_SET);
			jonj_stream_putc(dba->fp, 0);
			jonj_stream_flush(dba->fp);
			jonj_stream_seek(dba->fp, 0L, SEEK_END);
			efree(buf);
			return SUCCESS;
		}	

		/* read in the length of the value */
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		/* read in the value */
		num = jonj_stream_read(dba->fp, buf, num);
	}
	efree(buf);
	return FAILURE;
}	
/* }}} */

/* {{{ flatfile_findkey
 */
int flatfile_findkey(flatfile *dba, datum key_datum TSRMLS_DC) {
	size_t buf_size = FLATFILE_BLOCK_SIZE;
	char *buf = emalloc(buf_size);
	size_t num;
	int ret=0;
	void *key = key_datum.dptr;
	size_t size = key_datum.dsize;

	jonj_stream_rewind(dba->fp);
	while (!jonj_stream_eof(dba->fp)) {
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		num = jonj_stream_read(dba->fp, buf, num);

		if (size == num) {
			if (!memcmp(buf, key, size)) {
				ret = 1;
				break;
			}
		}	
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		num = jonj_stream_read(dba->fp, buf, num);
	}
	efree(buf);
	return ret;
}
/* }}} */

/* {{{ flatfile_firstkey
 */
datum flatfile_firstkey(flatfile *dba TSRMLS_DC) {
	datum res;
	size_t num;
	size_t buf_size = FLATFILE_BLOCK_SIZE;
	char *buf = emalloc(buf_size);

	jonj_stream_rewind(dba->fp);
	while(!jonj_stream_eof(dba->fp)) {
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		num = jonj_stream_read(dba->fp, buf, num);

		if (*(buf) != 0) {
			dba->CurrentFlatFilePos = jonj_stream_tell(dba->fp);
			res.dptr = buf;
			res.dsize = num;
			return res;
		}
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		num = jonj_stream_read(dba->fp, buf, num);
	}
	efree(buf);
	res.dptr = NULL;
	res.dsize = 0;
	return res;
}
/* }}} */

/* {{{ flatfile_nextkey
 */
datum flatfile_nextkey(flatfile *dba TSRMLS_DC) {
	datum res;
	size_t num;
	size_t buf_size = FLATFILE_BLOCK_SIZE;
	char *buf = emalloc(buf_size);

	jonj_stream_seek(dba->fp, dba->CurrentFlatFilePos, SEEK_SET);
	while(!jonj_stream_eof(dba->fp)) {
		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		num = jonj_stream_read(dba->fp, buf, num);

		if (!jonj_stream_gets(dba->fp, buf, 15)) {
			break;
		}
		num = atoi(buf);
		if (num >= buf_size) {
			buf_size = num + FLATFILE_BLOCK_SIZE;
			buf = erealloc(buf, buf_size);
		}
		num = jonj_stream_read(dba->fp, buf, num);

		if (*(buf)!=0) {
			dba->CurrentFlatFilePos = jonj_stream_tell(dba->fp);
			res.dptr = buf;
			res.dsize = num;
			return res;
		}
	}
	efree(buf);
	res.dptr = NULL;
	res.dsize = 0;
	return res;
}	
/* }}} */

/* {{{ flatfile_version */
char *flatfile_version() 
{
	return "1.0, $Id$";
}
/* }}} */ 

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
