/*
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@jonj.tk>                          |
   +----------------------------------------------------------------------+
 */

#ifndef RULEBASEDBREAKITERATOR_METHODS_H
#define RULEBASEDBREAKITERATOR_METHODS_H

#include <jonj.h>

JONJ_METHOD(IntlRuleBasedBreakIterator, __construct);

JONJ_FUNCTION(rbbi_get_rules);

JONJ_FUNCTION(rbbi_get_rule_status);

JONJ_FUNCTION(rbbi_get_rule_status_vec);

JONJ_FUNCTION(rbbi_get_binary_rules);

#endif