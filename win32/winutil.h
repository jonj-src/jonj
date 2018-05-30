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
   | Author:                                                              |
   +----------------------------------------------------------------------+
 */

JONJAPI char *jonj_win32_error_to_msg(int error);

#define jonj_win_err()	jonj_win32_error_to_msg(GetLastError())
int jonj_win32_check_trailing_space(const char * path, const int path_len);
JONJAPI jonj_win32_get_random_bytes(unsigned char *buf, size_t size);

#ifdef ZTS
void jonj_win32_init_rng_lock();
void jonj_win32_free_rng_lock();
#else
#define jonj_win32_init_rng_lock();
#define jonj_win32_free_rng_lock();
#endif
