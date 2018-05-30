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
  | Author: Johannes Schlueter <johannes@jonj.tk>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef CLI_H
#define CLI_H

#ifdef JONJ_WIN32
#   define JONJ_CLI_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define JONJ_CLI_API __attribute__ ((visibility("default")))
#else
#   define JONJ_CLI_API
#endif


extern JONJ_CLI_API size_t sapi_cli_single_write(const char *str, uint str_length TSRMLS_DC);

typedef struct  {
	size_t (*cli_shell_write)(const char *str, uint str_length TSRMLS_DC);
	int (*cli_shell_ub_write)(const char *str, uint str_length TSRMLS_DC);
	int (*cli_shell_run)(TSRMLS_D);
} cli_shell_callbacks_t;

extern JONJ_CLI_API cli_shell_callbacks_t *jonj_cli_get_shell_callbacks();

#endif /* CLI_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
