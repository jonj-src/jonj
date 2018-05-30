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
   | Authors: Felipe Pena <felipe@jonj.tk>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@jonj.tk>                                |
   +----------------------------------------------------------------------+
*/

#ifndef JONJDBG_OPCODE_H
#define JONJDBG_OPCODE_H

#include "zend_types.h"

const char *jonjdbg_decode_opcode(zend_uchar);
char *jonjdbg_decode_opline(zend_op_array *ops, zend_op *op, HashTable *vars TSRMLS_DC);
void jonjdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags TSRMLS_DC);
void jonjdbg_print_opline_ex(zend_execute_data *execute_data, HashTable *vars, zend_bool ignore_flags TSRMLS_DC);

#endif /* JONJDBG_OPCODE_H */
