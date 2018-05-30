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
   | Author: Andi Gutmans <andi@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "jonj.h"

#if HAVE_BCMATH

#include "jonj_ini.h"
#include "ext/standard/info.h"
#include "jonj_bcmath.h"
#include "libbcmath/src/bcmath.h"

ZEND_DECLARE_MODULE_GLOBALS(bcmath)
static JONJ_GINIT_FUNCTION(bcmath);
static JONJ_GSHUTDOWN_FUNCTION(bcmath);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_bcadd, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcsub, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcmul, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcdiv, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_bcmod, 0)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcpowmod, 0, 0, 3)
	ZEND_ARG_INFO(0, x)
	ZEND_ARG_INFO(0, y)
	ZEND_ARG_INFO(0, mod)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcpow, 0, 0, 2)
	ZEND_ARG_INFO(0, x)
	ZEND_ARG_INFO(0, y)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bcsqrt, 0, 0, 1)
	ZEND_ARG_INFO(0, operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bccomp, 0, 0, 2)
	ZEND_ARG_INFO(0, left_operand)
	ZEND_ARG_INFO(0, right_operand)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_bcscale, 0)
	ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()

/* }}} */

const zend_function_entry bcmath_functions[] = {
	JONJ_FE(bcadd,									arginfo_bcadd)
	JONJ_FE(bcsub,									arginfo_bcsub)
	JONJ_FE(bcmul,									arginfo_bcmul)
	JONJ_FE(bcdiv,									arginfo_bcdiv)
	JONJ_FE(bcmod,									arginfo_bcmod)
	JONJ_FE(bcpow,									arginfo_bcpow)
	JONJ_FE(bcsqrt,									arginfo_bcsqrt)
	JONJ_FE(bcscale,									arginfo_bcscale)
	JONJ_FE(bccomp,									arginfo_bccomp)
	JONJ_FE(bcpowmod,								arginfo_bcpowmod)
	JONJ_FE_END
};

zend_module_entry bcmath_module_entry = {
	STANDARD_MODULE_HEADER,
	"bcmath",
	bcmath_functions,
	JONJ_MINIT(bcmath),
	JONJ_MSHUTDOWN(bcmath),
	NULL,
	NULL,
	JONJ_MINFO(bcmath),
	NO_VERSION_YET,
	JONJ_MODULE_GLOBALS(bcmath),
	JONJ_GINIT(bcmath),
    JONJ_GSHUTDOWN(bcmath),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_BCMATH
ZEND_GET_MODULE(bcmath)
#endif

/* {{{ JONJ_INI */
JONJ_INI_BEGIN()
	STD_JONJ_INI_ENTRY("bcmath.scale", "0", JONJ_INI_ALL, OnUpdateLongGEZero, bc_precision, zend_bcmath_globals, bcmath_globals)
JONJ_INI_END()
/* }}} */

/* {{{ JONJ_GINIT_FUNCTION
 */
static JONJ_GINIT_FUNCTION(bcmath)
{
	bcmath_globals->bc_precision = 0;
	bc_init_numbers(TSRMLS_C);
}
/* }}} */

/* {{{ JONJ_GSHUTDOWN_FUNCTION
 */
static JONJ_GSHUTDOWN_FUNCTION(bcmath)
{
	_bc_free_num_ex(&bcmath_globals->_zero_, 1);
	_bc_free_num_ex(&bcmath_globals->_one_, 1);
	_bc_free_num_ex(&bcmath_globals->_two_, 1);
}
/* }}} */

/* {{{ JONJ_MINIT_FUNCTION
 */
JONJ_MINIT_FUNCTION(bcmath)
{
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MSHUTDOWN_FUNCTION
 */
JONJ_MSHUTDOWN_FUNCTION(bcmath)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ JONJ_MINFO_FUNCTION
 */
JONJ_MINFO_FUNCTION(bcmath)
{
	jonj_info_print_table_start();
	jonj_info_print_table_row(2, "BCMath support", "enabled");
	jonj_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ jonj_str2num
   Convert to bc_num detecting scale */
static void jonj_str2num(bc_num *num, char *str TSRMLS_DC)
{
	char *p;

	if (!(p = strchr(str, '.'))) {
		bc_str2num(num, str, 0 TSRMLS_CC);
		return;
	}

	bc_str2num(num, str, strlen(p+1) TSRMLS_CC);
}
/* }}} */

/* {{{ split_bc_num
   Convert to bc_num detecting scale */
static bc_num split_bc_num(bc_num num) {
	bc_num newnum;
	if (num->n_refs >= 1) {
		return num;
	}
	newnum = _bc_new_num_ex(0, 0, 0);
	*newnum = *num;
	newnum->n_refs = 1;
	num->n_refs--;
	return newnum;
}
/* }}} */

/* {{{ proto string bcadd(string left_operand, string right_operand [, int scale])
   Returns the sum of two arbitrary precision numbers */
JONJ_FUNCTION(bcadd)
{
	char *left, *right;
	long scale_param = 0;
	bc_num first, second, result;
	int left_len, right_len;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", &left, &left_len, &right, &right_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 3) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&first, left TSRMLS_CC);
	jonj_str2num(&second, right TSRMLS_CC);
	bc_add (first, second, &result, scale);

	if (result->n_scale > scale) {
		result = split_bc_num(result);
		result->n_scale = scale;
	}

	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcsub(string left_operand, string right_operand [, int scale])
   Returns the difference between two arbitrary precision numbers */
JONJ_FUNCTION(bcsub)
{
	char *left, *right;
	int left_len, right_len;
	long scale_param = 0;
	bc_num first, second, result;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", &left, &left_len, &right, &right_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 3) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&first, left TSRMLS_CC);
	jonj_str2num(&second, right TSRMLS_CC);
	bc_sub (first, second, &result, scale);

	if (result->n_scale > scale) {
		result = split_bc_num(result);
		result->n_scale = scale;
	}

	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmul(string left_operand, string right_operand [, int scale])
   Returns the multiplication of two arbitrary precision numbers */
JONJ_FUNCTION(bcmul)
{
	char *left, *right;
	int left_len, right_len;
	long scale_param = 0;
	bc_num first, second, result;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", &left, &left_len, &right, &right_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 3) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&first, left TSRMLS_CC);
	jonj_str2num(&second, right TSRMLS_CC);
	bc_multiply (first, second, &result, scale TSRMLS_CC);

	if (result->n_scale > scale) {
		result = split_bc_num(result);
		result->n_scale = scale;
	}

	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcdiv(string left_operand, string right_operand [, int scale])
   Returns the quotient of two arbitrary precision numbers (division) */
JONJ_FUNCTION(bcdiv)
{
	char *left, *right;
	int left_len, right_len;
	long scale_param = 0;
	bc_num first, second, result;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", &left, &left_len, &right, &right_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 3) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&first, left TSRMLS_CC);
	jonj_str2num(&second, right TSRMLS_CC);

	switch (bc_divide(first, second, &result, scale TSRMLS_CC)) {
		case 0: /* OK */
			if (result->n_scale > scale) {
				result = split_bc_num(result);
				result->n_scale = scale;
			}
			Z_STRVAL_P(return_value) = bc_num2str(result);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case -1: /* division by zero */
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Division by zero");
			break;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmod(string left_operand, string right_operand)
   Returns the modulus of the two arbitrary precision operands */
JONJ_FUNCTION(bcmod)
{
	char *left, *right;
	int left_len, right_len;
	bc_num first, second, result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &left, &left_len, &right, &right_len) == FAILURE) {
		return;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	bc_str2num(&first, left, 0 TSRMLS_CC);
	bc_str2num(&second, right, 0 TSRMLS_CC);

	switch (bc_modulo(first, second, &result, 0 TSRMLS_CC)) {
		case 0:
			Z_STRVAL_P(return_value) = bc_num2str(result);
			Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
			Z_TYPE_P(return_value) = IS_STRING;
			break;
		case -1:
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Division by zero");
			break;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcpowmod(string x, string y, string mod [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another reduced by a modulous */
JONJ_FUNCTION(bcpowmod)
{
	char *left, *right, *modulous;
	int left_len, right_len, modulous_len;
	bc_num first, second, mod, result;
	long scale = BCG(bc_precision);
	int scale_int;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|l", &left, &left_len, &right, &right_len, &modulous, &modulous_len, &scale) == FAILURE) {
		return;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&mod TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&first, left TSRMLS_CC);
	jonj_str2num(&second, right TSRMLS_CC);
	jonj_str2num(&mod, modulous TSRMLS_CC);

	scale_int = (int) ((int)scale < 0) ? 0 : scale;

	if (bc_raisemod(first, second, mod, &result, scale_int TSRMLS_CC) != -1) {
		if (result->n_scale > scale_int) {
			result = split_bc_num(result);
			result->n_scale = scale_int;
		}
		Z_STRVAL_P(return_value) = bc_num2str(result);
		Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
		Z_TYPE_P(return_value) = IS_STRING;
	} else {
		RETVAL_FALSE;
	}

	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&mod);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcpow(string x, string y [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another */
JONJ_FUNCTION(bcpow)
{
	char *left, *right;
	int left_len, right_len;
	long scale_param = 0;
	bc_num first, second, result;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", &left, &left_len, &right, &right_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 3) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);
	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&first, left TSRMLS_CC);
	jonj_str2num(&second, right TSRMLS_CC);
	bc_raise (first, second, &result, scale TSRMLS_CC);

	if (result->n_scale > scale) {
		result = split_bc_num(result);
		result->n_scale = scale;
	}

	Z_STRVAL_P(return_value) = bc_num2str(result);
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
	bc_free_num(&first);
	bc_free_num(&second);
	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcsqrt(string operand [, int scale])
   Returns the square root of an arbitray precision number */
JONJ_FUNCTION(bcsqrt)
{
	char *left;
	int left_len;
	long scale_param = 0;
	bc_num result;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "s|l", &left, &left_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 2) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&result TSRMLS_CC);
	jonj_str2num(&result, left TSRMLS_CC);

	if (bc_sqrt (&result, scale TSRMLS_CC) != 0) {
		if (result->n_scale > scale) {
			result = split_bc_num(result);
			result->n_scale = scale;
		}
		Z_STRVAL_P(return_value) = bc_num2str(result);
		Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
		Z_TYPE_P(return_value) = IS_STRING;
	} else {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Square root of negative number");
	}

	bc_free_num(&result);
	return;
}
/* }}} */

/* {{{ proto int bccomp(string left_operand, string right_operand [, int scale])
   Compares two arbitrary precision numbers */
JONJ_FUNCTION(bccomp)
{
	char *left, *right;
	int left_len, right_len;
	long scale_param = 0;
	bc_num first, second;
	int scale = BCG(bc_precision), argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "ss|l", &left, &left_len, &right, &right_len, &scale_param) == FAILURE) {
		return;
	}

	if (argc == 3) {
		scale = (int) ((int)scale_param < 0) ? 0 : scale_param;
	}

	bc_init_num(&first TSRMLS_CC);
	bc_init_num(&second TSRMLS_CC);

	bc_str2num(&first, left, scale TSRMLS_CC);
	bc_str2num(&second, right, scale TSRMLS_CC);
	Z_LVAL_P(return_value) = bc_compare(first, second);
	Z_TYPE_P(return_value) = IS_LONG;

	bc_free_num(&first);
	bc_free_num(&second);
	return;
}
/* }}} */

/* {{{ proto bool bcscale(int scale)
   Sets default scale parameter for all bc math functions */
JONJ_FUNCTION(bcscale)
{
	long new_scale;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &new_scale) == FAILURE) {
		return;
	}

	BCG(bc_precision) = ((int)new_scale < 0) ? 0 : new_scale;

	RETURN_TRUE;
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */