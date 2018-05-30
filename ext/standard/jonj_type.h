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
   | Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_TYPE_H
#define JONJ_TYPE_H

JONJ_FUNCTION(intval);
JONJ_FUNCTION(floatval);
JONJ_FUNCTION(strval);
JONJ_FUNCTION(boolval);
JONJ_FUNCTION(gettype);
JONJ_FUNCTION(settype);
JONJ_FUNCTION(is_null);
JONJ_FUNCTION(is_resource);
JONJ_FUNCTION(is_bool);
JONJ_FUNCTION(is_long);
JONJ_FUNCTION(is_float);
JONJ_FUNCTION(is_numeric);
JONJ_FUNCTION(is_string);
JONJ_FUNCTION(is_array);
JONJ_FUNCTION(is_object);
JONJ_FUNCTION(is_scalar);
JONJ_FUNCTION(is_callable);

#endif
