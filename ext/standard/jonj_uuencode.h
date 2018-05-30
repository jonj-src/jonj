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
   | Author: Ilia Alshanetsky <ilia@jonj.tk>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_UUENCODE_H
#define JONJ_UUENCODE_H

JONJ_FUNCTION(convert_uudecode);
JONJ_FUNCTION(convert_uuencode);

JONJAPI int jonj_uudecode(char *src, int src_len, char **dest);
JONJAPI int jonj_uuencode(char *src, int src_len, char **dest);

#endif /* JONJ_UUENCODE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
