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
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   | JONJ 4.0 patches by:                                                  |
   | Zeev Suraski <zeev@zend.com>                                         |
   | Stig Bakken <ssb@jonj.tk>                                            |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "jonj_apache_http.h"

/* {{{ apache_jonj_module_main
 */
int apache_jonj_module_main(request_rec *r, int display_source_mode TSRMLS_DC)
{
	int retval = OK;	
	zend_file_handle file_handle;

	if (jonj_request_startup(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}
	/* sending a file handle to another dll is not working
	   so let zend open it. */
	
	if (display_source_mode) {
		zend_syntax_highlighter_ini syntax_highlighter_ini;

		jonj_get_highlight_struct(&syntax_highlighter_ini);
		if (highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC) != SUCCESS) {
			retval = NOT_FOUND;
		}
	} else {
		file_handle.type = ZEND_HANDLE_FILENAME;
		file_handle.handle.fd = 0;
		file_handle.filename = SG(request_info).path_translated;
		file_handle.opened_path = NULL;
		file_handle.free_filename = 0;

		(void) jonj_execute_script(&file_handle TSRMLS_CC);
	}

	AP(in_request) = 0;
	
	zend_try {
		jonj_request_shutdown(NULL);
	} zend_end_try();
	
	return retval;
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
