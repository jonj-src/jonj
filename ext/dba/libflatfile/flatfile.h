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
   | Author: Marcus Boerger <helly@jonj.tk>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_LIB_FLATFILE_H
#define JONJ_LIB_FLATFILE_H

typedef struct {
	char *dptr;
	size_t dsize;
} datum;

typedef struct {
	char *lockfn;
	int lockfd;
	jonj_stream *fp;
	size_t CurrentFlatFilePos;
	datum nextkey;
} flatfile;

#define FLATFILE_INSERT 1
#define FLATFILE_REPLACE 0

int flatfile_store(flatfile *dba, datum key_datum, datum value_datum, int mode TSRMLS_DC);
datum flatfile_fetch(flatfile *dba, datum key_datum TSRMLS_DC);
int flatfile_delete(flatfile *dba, datum key_datum TSRMLS_DC);
int flatfile_findkey(flatfile *dba, datum key_datum TSRMLS_DC);
datum flatfile_firstkey(flatfile *dba TSRMLS_DC);
datum flatfile_nextkey(flatfile *dba TSRMLS_DC);
char *flatfile_version();

#endif
