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
   | Authors: Jim Winstead <jimw@jonj.tk>                                 |
   |          Stig Sæther Bakken <ssb@jonj.tk>                            |
   |          Zeev Suraski <zeev@zend.com>                                |
   | JONJ 4.0 patches by Thies C. Arntzen <thies@thieso.net>               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj.h"
#include "jonj_math.h"
#include "zend_multiply.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "basic_functions.h"

/* {{{ jonj_intlog10abs
   Returns floor(log10(fabs(val))), uses fast binary search */
static inline int jonj_intlog10abs(double value) {
	int result;
	value = fabs(value);

	if (value < 1e-8 || value > 1e22) {
		result = (int)floor(log10(value));
	} else {
		static const double values[] = {
			1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1,
			1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
			1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
			1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};
		/* Do a binary search with 5 steps */
		result = 15;
		if (value < values[result]) {
			result -= 8;
		} else {
			result += 8;
		}
		if (value < values[result]) {
			result -= 4;
		} else {
			result += 4;
		}
		if (value < values[result]) {
			result -= 2;
		} else {
			result += 2;
		}
		if (value < values[result]) {
			result -= 1;
		} else {
			result += 1;
		}
		if (value < values[result]) {
			result -= 1;
		}
		result -= 8;
	}
	return result;
}
/* }}} */

/* {{{ jonj_intpow10
       Returns pow(10.0, (double)power), uses fast lookup table for exact powers */
static inline double jonj_intpow10(int power) {
	static const double powers[] = {
		1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
		1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
		1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

	/* Not in lookup table */
	if (power < 0 || power > 22) {
		return pow(10.0, (double)power);
	}
	return powers[power];
}
/* }}} */

/* {{{ jonj_math_is_finite */
static inline int jonj_math_is_finite(double value) {
#if defined(JONJ_WIN32)
	return _finite(value);
#elif defined(isfinite)
	return isfinite(value);
#else
	return value == value && (value == 0. || value * 2. != value);
#endif
}
/* }}} */

/* {{{ jonj_round_helper
       Actually performs the rounding of a value to integer in a certain mode */
static inline double jonj_round_helper(double value, int mode) {
	double tmp_value;

	if (value >= 0.0) {
		tmp_value = floor(value + 0.5);
		if ((mode == JONJ_ROUND_HALF_DOWN && value == (-0.5 + tmp_value)) ||
			(mode == JONJ_ROUND_HALF_EVEN && value == (0.5 + 2 * floor(tmp_value/2.0))) ||
			(mode == JONJ_ROUND_HALF_ODD  && value == (0.5 + 2 * floor(tmp_value/2.0) - 1.0)))
		{
			tmp_value = tmp_value - 1.0;
		}
	} else {
		tmp_value = ceil(value - 0.5);
		if ((mode == JONJ_ROUND_HALF_DOWN && value == (0.5 + tmp_value)) ||
			(mode == JONJ_ROUND_HALF_EVEN && value == (-0.5 + 2 * ceil(tmp_value/2.0))) ||
			(mode == JONJ_ROUND_HALF_ODD  && value == (-0.5 + 2 * ceil(tmp_value/2.0) + 1.0)))
		{
			tmp_value = tmp_value + 1.0;
		}
	}

	return tmp_value;
}
/* }}} */

/* {{{ _jonj_math_round */
/*
 * Rounds a number to a certain number of decimal places in a certain rounding
 * mode. For the specifics of the algorithm, see http://wiki.jonj.tk/rfc/rounding
 */
JONJAPI double _jonj_math_round(double value, int places, int mode) {
	double f1, f2;
	double tmp_value;
	int precision_places;

	if (!jonj_math_is_finite(value)) {
		return value;
	}

	places = places < INT_MIN+1 ? INT_MIN+1 : places;
	precision_places = 14 - jonj_intlog10abs(value);

	f1 = jonj_intpow10(abs(places));

	/* If the decimal precision guaranteed by FP arithmetic is higher than
	   the requested places BUT is small enough to make sure a non-zero value
	   is returned, pre-round the result to the precision */
	if (precision_places > places && precision_places - places < 15) {
		int64_t use_precision = precision_places < INT_MIN+1 ? INT_MIN+1 : precision_places;

		f2 = jonj_intpow10(abs((int)use_precision));
		if (use_precision >= 0) {
			tmp_value = value * f2;
		} else {
			tmp_value = value / f2;
		}
		/* preround the result (tmp_value will always be something * 1e14,
		   thus never larger than 1e15 here) */
		tmp_value = jonj_round_helper(tmp_value, mode);

		use_precision = places - precision_places;
		use_precision = use_precision < INT_MIN+1 ? INT_MIN+1 : use_precision;
		/* now correctly move the decimal point */
		f2 = jonj_intpow10(abs((int)use_precision));
		/* because places < precision_places */
		tmp_value = tmp_value / f2;
	} else {
		/* adjust the value */
		if (places >= 0) {
			tmp_value = value * f1;
		} else {
			tmp_value = value / f1;
		}
		/* This value is beyond our precision, so rounding it is pointless */
		if (fabs(tmp_value) >= 1e15) {
			return value;
		}
	}

	/* round the temp value */
	tmp_value = jonj_round_helper(tmp_value, mode);

	/* see if it makes sense to use simple division to round the value */
	if (abs(places) < 23) {
		if (places > 0) {
			tmp_value = tmp_value / f1;
		} else {
			tmp_value = tmp_value * f1;
		}
	} else {
		/* Simple division can't be used since that will cause wrong results.
		   Instead, the number is converted to a string and back again using
		   strtod(). strtod() will return the nearest possible FP value for
		   that string. */

		/* 40 Bytes should be more than enough for this format string. The
		   float won't be larger than 1e15 anyway. But just in case, use
		   snprintf() and make sure the buffer is zero-terminated */
		char buf[40];
		snprintf(buf, 39, "%15fe%d", tmp_value, -places);
		buf[39] = '\0';
		tmp_value = zend_strtod(buf, NULL);
		/* couldn't convert to string and back */
		if (!zend_finite(tmp_value) || zend_isnan(tmp_value)) {
			tmp_value = value;
		}
	}

	return tmp_value;
}
/* }}} */

/* {{{ jonj_asinh
*/
static double jonj_asinh(double z)
{
#ifdef HAVE_ASINH
	return(asinh(z));
#else
	return(log(z + sqrt(1 + pow(z, 2))) / log(M_E));
#endif
}
/* }}} */

/* {{{ jonj_acosh
*/
static double jonj_acosh(double x)
{
#ifdef HAVE_ACOSH
	return(acosh(x));
#else
	return(log(x + sqrt(x * x - 1)));
#endif
}
/* }}} */

/* {{{ jonj_atanh
*/
static double jonj_atanh(double z)
{
#ifdef HAVE_ATANH
	return(atanh(z));
#else
	return(0.5 * log((1 + z) / (1 - z)));
#endif
}
/* }}} */

/* {{{ jonj_log1p
*/
static double jonj_log1p(double x)
{
#ifdef HAVE_LOG1P
	return(log1p(x));
#else
	return(log(1 + x));
#endif
}
/* }}} */

/* {{{ jonj_expm1
*/
static double jonj_expm1(double x)
{
#if !defined(JONJ_WIN32) && !defined(NETWARE)
	return(expm1(x));
#else
	return(exp(x) - 1);
#endif
}
/* }}}*/

/* {{{ proto int abs(int number)
   Return the absolute value of the number */
JONJ_FUNCTION(abs)
{
	zval **value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);

	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(fabs(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		if (Z_LVAL_PP(value) == LONG_MIN) {
			RETURN_DOUBLE(-(double)LONG_MIN);
		} else {
			RETURN_LONG(Z_LVAL_PP(value) < 0 ? -Z_LVAL_PP(value) : Z_LVAL_PP(value));
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float ceil(float number)
   Returns the next highest integer value of the number */
JONJ_FUNCTION(ceil)
{
	zval **value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);

	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(ceil(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		convert_to_double_ex(value);
		RETURN_DOUBLE(Z_DVAL_PP(value));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float floor(float number)
   Returns the next lowest integer value from the number */
JONJ_FUNCTION(floor)
{
	zval **value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &value) == FAILURE) {
		return;
	}
	convert_scalar_to_number_ex(value);

	if (Z_TYPE_PP(value) == IS_DOUBLE) {
		RETURN_DOUBLE(floor(Z_DVAL_PP(value)));
	} else if (Z_TYPE_PP(value) == IS_LONG) {
		convert_to_double_ex(value);
		RETURN_DOUBLE(Z_DVAL_PP(value));
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float round(float number [, int precision [, int mode]])
   Returns the number rounded to specified precision */
JONJ_FUNCTION(round)
{
	zval **value;
	int places = 0;
	long precision = 0;
	long mode = JONJ_ROUND_HALF_UP;
	double return_val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|ll", &value, &precision, &mode) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() >= 2) {
#if SIZEOF_LONG > SIZEOF_INT
		if (precision >= 0) {
			places = precision > INT_MAX ? INT_MAX : (int)precision;
		} else {
			places = precision <= INT_MIN ? INT_MIN+1 : (int)precision;
		}
#else
		places = precision;
#endif
	}
	convert_scalar_to_number_ex(value);

	switch (Z_TYPE_PP(value)) {
		case IS_LONG:
			/* Simple case - long that doesn't need to be rounded. */
			if (places >= 0) {
				RETURN_DOUBLE((double) Z_LVAL_PP(value));
			}
			/* break omitted intentionally */

		case IS_DOUBLE:
			return_val = (Z_TYPE_PP(value) == IS_LONG) ? (double)Z_LVAL_PP(value) : Z_DVAL_PP(value);
			return_val = _jonj_math_round(return_val, places, mode);
			RETURN_DOUBLE(return_val);
			break;

		default:
			RETURN_FALSE;
			break;
	}
}
/* }}} */

/* {{{ proto float sin(float number)
   Returns the sine of the number in radians */
JONJ_FUNCTION(sin)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(sin(num));
}
/* }}} */

/* {{{ proto float cos(float number)
   Returns the cosine of the number in radians */
JONJ_FUNCTION(cos)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(cos(num));
}
/* }}} */

/* {{{ proto float tan(float number)
   Returns the tangent of the number in radians */
JONJ_FUNCTION(tan)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(tan(num));
}
/* }}} */

/* {{{ proto float asin(float number)
   Returns the arc sine of the number in radians */
JONJ_FUNCTION(asin)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(asin(num));
}
/* }}} */

/* {{{ proto float acos(float number)
   Return the arc cosine of the number in radians */
JONJ_FUNCTION(acos)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(acos(num));
}
/* }}} */

/* {{{ proto float atan(float number)
   Returns the arc tangent of the number in radians */
JONJ_FUNCTION(atan)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(atan(num));
}
/* }}} */

/* {{{ proto float atan2(float y, float x)
   Returns the arc tangent of y/x, with the resulting quadrant determined by the signs of y and x */
JONJ_FUNCTION(atan2)
{
	double num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &num1, &num2) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(atan2(num1, num2));
}
/* }}} */

/* {{{ proto float sinh(float number)
   Returns the hyperbolic sine of the number, defined as (exp(number) - exp(-number))/2 */
JONJ_FUNCTION(sinh)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(sinh(num));
}
/* }}} */

/* {{{ proto float cosh(float number)
   Returns the hyperbolic cosine of the number, defined as (exp(number) + exp(-number))/2 */
JONJ_FUNCTION(cosh)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(cosh(num));
}
/* }}} */

/* {{{ proto float tanh(float number)
   Returns the hyperbolic tangent of the number, defined as sinh(number)/cosh(number) */
JONJ_FUNCTION(tanh)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(tanh(num));
}
/* }}} */

/* {{{ proto float asinh(float number)
   Returns the inverse hyperbolic sine of the number, i.e. the value whose hyperbolic sine is number */
JONJ_FUNCTION(asinh)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(jonj_asinh(num));
}
/* }}} */

/* {{{ proto float acosh(float number)
   Returns the inverse hyperbolic cosine of the number, i.e. the value whose hyperbolic cosine is number */
JONJ_FUNCTION(acosh)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(jonj_acosh(num));
}
/* }}} */

/* {{{ proto float atanh(float number)
   Returns the inverse hyperbolic tangent of the number, i.e. the value whose hyperbolic tangent is number */
JONJ_FUNCTION(atanh)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(jonj_atanh(num));
}
/* }}} */

/* {{{ proto float pi(void)
   Returns an approximation of pi */
JONJ_FUNCTION(pi)
{
	RETURN_DOUBLE(M_PI);
}
/* }}} */

/* {{{ proto bool is_finite(float val)
   Returns whether argument is finite */
JONJ_FUNCTION(is_finite)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_finite(dval));
}
/* }}} */

/* {{{ proto bool is_infinite(float val)
   Returns whether argument is infinite */
JONJ_FUNCTION(is_infinite)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_isinf(dval));
}
/* }}} */

/* {{{ proto bool is_nan(float val)
   Returns whether argument is not a number */
JONJ_FUNCTION(is_nan)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_isnan(dval));
}
/* }}} */

/* {{{ proto number pow(number base, number exponent)
   Returns base raised to the power of exponent. Returns integer result when possible */
JONJ_FUNCTION(pow)
{
	zval *zbase, *zexp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z/", &zbase, &zexp) == FAILURE) {
		return;
	}

	pow_function(return_value, zbase, zexp TSRMLS_CC);
}
/* }}} */

/* {{{ proto float exp(float number)
   Returns e raised to the power of the number */
JONJ_FUNCTION(exp)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}

	RETURN_DOUBLE(exp(num));
}
/* }}} */

/* {{{ proto float expm1(float number)
   Returns exp(number) - 1, computed in a way that accurate even when the value of number is close to zero */
/*
   WARNING: this function is expermental: it could change its name or
   disappear in the next version of JONJ!
*/
JONJ_FUNCTION(expm1)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(jonj_expm1(num));
}
/* }}} */

/* {{{ proto float log1p(float number)
   Returns log(1 + number), computed in a way that accurate even when the value of number is close to zero */
/*
   WARNING: this function is expermental: it could change its name or
   disappear in the next version of JONJ!
*/
JONJ_FUNCTION(log1p)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(jonj_log1p(num));
}
/* }}} */

/* {{{ proto float log(float number, [float base])
   Returns the natural logarithm of the number, or the base log if base is specified */
JONJ_FUNCTION(log)
{
	double num, base = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d|d", &num, &base) == FAILURE) {
		return;
	}
	if (ZEND_NUM_ARGS() == 1) {
		RETURN_DOUBLE(log(num));
	}
	if (base <= 0.0) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "base must be greater than 0");
		RETURN_FALSE;
	}
	if (base == 1) {
		RETURN_DOUBLE(jonj_get_nan());
	} else {
		RETURN_DOUBLE(log(num) / log(base));
	}
}
/* }}} */

/* {{{ proto float log10(float number)
   Returns the base-10 logarithm of the number */
JONJ_FUNCTION(log10)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(log10(num));
}
/* }}} */

/* {{{ proto float sqrt(float number)
   Returns the square root of the number */
JONJ_FUNCTION(sqrt)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(sqrt(num));
}
/* }}} */

/* {{{ proto float hypot(float num1, float num2)
   Returns sqrt(num1*num1 + num2*num2) */
JONJ_FUNCTION(hypot)
{
	double num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &num1, &num2) == FAILURE) {
		return;
	}
#if HAVE_HYPOT
	RETURN_DOUBLE(hypot(num1, num2));
#elif defined(_MSC_VER)
	RETURN_DOUBLE(_hypot(num1, num2));
#else
	RETURN_DOUBLE(sqrt((num1 * num1) + (num2 * num2)));
#endif
}
/* }}} */

/* {{{ proto float deg2rad(float number)
   Converts the number in degrees to the radian equivalent */
JONJ_FUNCTION(deg2rad)
{
	double deg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &deg) == FAILURE) {
		return;
	}
	RETURN_DOUBLE((deg / 180.0) * M_PI);
}
/* }}} */

/* {{{ proto float rad2deg(float number)
   Converts the radian number to the equivalent number in degrees */
JONJ_FUNCTION(rad2deg)
{
	double rad;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &rad) == FAILURE) {
		return;
	}
	RETURN_DOUBLE((rad / M_PI) * 180);
}
/* }}} */

/* {{{ _jonj_math_basetolong */
/*
 * Convert a string representation of a base(2-36) number to a long.
 */
JONJAPI long _jonj_math_basetolong(zval *arg, int base)
{
	long num = 0, digit, onum;
	int i;
	char c, *s;

	if (Z_TYPE_P(arg) != IS_STRING || base < 2 || base > 36) {
		return 0;
	}

	s = Z_STRVAL_P(arg);

	for (i = Z_STRLEN_P(arg); i > 0; i--) {
		c = *s++;

		digit = (c >= '0' && c <= '9') ? c - '0'
			: (c >= 'A' && c <= 'Z') ? c - 'A' + 10
			: (c >= 'a' && c <= 'z') ? c - 'a' + 10
			: base;

		if (digit >= base) {
			continue;
		}

		onum = num;
		num = num * base + digit;
		if (num > onum)
			continue;

		{
			TSRMLS_FETCH();

			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Number '%s' is too big to fit in long", s);
			return LONG_MAX;
		}
	}

	return num;
}
/* }}} */

/* {{{ _jonj_math_basetozval */
/*
 * Convert a string representation of a base(2-36) number to a zval.
 */
JONJAPI int _jonj_math_basetozval(zval *arg, int base, zval *ret)
{
	long num = 0;
	double fnum = 0;
	int i;
	int mode = 0;
	char c, *s;
	long cutoff;
	int cutlim;

	if (Z_TYPE_P(arg) != IS_STRING || base < 2 || base > 36) {
		return FAILURE;
	}

	s = Z_STRVAL_P(arg);

	cutoff = LONG_MAX / base;
	cutlim = LONG_MAX % base;

	for (i = Z_STRLEN_P(arg); i > 0; i--) {
		c = *s++;

		/* might not work for EBCDIC */
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			continue;

		if (c >= base)
			continue;

		switch (mode) {
		case 0: /* Integer */
			if (num < cutoff || (num == cutoff && c <= cutlim)) {
				num = num * base + c;
				break;
			} else {
				fnum = num;
				mode = 1;
			}
			/* fall-through */
		case 1: /* Float */
			fnum = fnum * base + c;
		}
	}

	if (mode == 1) {
		ZVAL_DOUBLE(ret, fnum);
	} else {
		ZVAL_LONG(ret, num);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ _jonj_math_longtobase */
/*
 * Convert a long to a string containing a base(2-36) representation of
 * the number.
 */
JONJAPI char * _jonj_math_longtobase(zval *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char buf[(sizeof(unsigned long) << 3) + 1];
	char *ptr, *end;
	unsigned long value;

	if (Z_TYPE_P(arg) != IS_LONG || base < 2 || base > 36) {
		return STR_EMPTY_ALLOC();
	}

	value = Z_LVAL_P(arg);

	end = ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';

	do {
		*--ptr = digits[value % base];
		value /= base;
	} while (ptr > buf && value);

	return estrndup(ptr, end - ptr);
}
/* }}} */

/* {{{ _jonj_math_zvaltobase */
/*
 * Convert a zval to a string containing a base(2-36) representation of
 * the number.
 */
JONJAPI char * _jonj_math_zvaltobase(zval *arg, int base TSRMLS_DC)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	if ((Z_TYPE_P(arg) != IS_LONG && Z_TYPE_P(arg) != IS_DOUBLE) || base < 2 || base > 36) {
		return STR_EMPTY_ALLOC();
	}

	if (Z_TYPE_P(arg) == IS_DOUBLE) {
		double fvalue = floor(Z_DVAL_P(arg)); /* floor it just in case */
		char *ptr, *end;
		char buf[(sizeof(double) << 3) + 1];

		/* Don't try to convert +/- infinity */
		if (fvalue == HUGE_VAL || fvalue == -HUGE_VAL) {
			jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Number too large");
			return STR_EMPTY_ALLOC();
		}

		end = ptr = buf + sizeof(buf) - 1;
		*ptr = '\0';

		do {
			*--ptr = digits[(int) fmod(fvalue, base)];
			fvalue /= base;
		} while (ptr > buf && fabs(fvalue) >= 1);

		return estrndup(ptr, end - ptr);
	}

	return _jonj_math_longtobase(arg, base);
}
/* }}} */

/* {{{ proto int bindec(string binary_number)
   Returns the decimal equivalent of the binary number */
JONJ_FUNCTION(bindec)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_jonj_math_basetozval(*arg, 2, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int hexdec(string hexadecimal_number)
   Returns the decimal equivalent of the hexadecimal number */
JONJ_FUNCTION(hexdec)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_jonj_math_basetozval(*arg, 16, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int octdec(string octal_number)
   Returns the decimal equivalent of an octal string */
JONJ_FUNCTION(octdec)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_jonj_math_basetozval(*arg, 8, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string decbin(int decimal_number)
   Returns a string containing a binary representation of the number */
JONJ_FUNCTION(decbin)
{
	zval **arg;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _jonj_math_longtobase(*arg, 2);
	RETURN_STRING(result, 0);
}
/* }}} */

/* {{{ proto string decoct(int decimal_number)
   Returns a string containing an octal representation of the given number */
JONJ_FUNCTION(decoct)
{
	zval **arg;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _jonj_math_longtobase(*arg, 8);
	RETURN_STRING(result, 0);
}
/* }}} */

/* {{{ proto string dechex(int decimal_number)
   Returns a string containing a hexadecimal representation of the given number */
JONJ_FUNCTION(dechex)
{
	zval **arg;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _jonj_math_longtobase(*arg, 16);
	RETURN_STRING(result, 0);
}
/* }}} */

/* {{{ proto string base_convert(string number, int frombase, int tobase)
   Converts a number in a string from any base <= 36 to any base <= 36 */
JONJ_FUNCTION(base_convert)
{
	zval **number, temp;
	long frombase, tobase;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zll", &number, &frombase, &tobase) == FAILURE) {
		return;
	}
	convert_to_string_ex(number);

	if (frombase < 2 || frombase > 36) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid `from base' (%ld)", frombase);
		RETURN_FALSE;
	}
	if (tobase < 2 || tobase > 36) {
		jonj_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid `to base' (%ld)", tobase);
		RETURN_FALSE;
	}

	if(_jonj_math_basetozval(*number, frombase, &temp) == FAILURE) {
		RETURN_FALSE;
	}
	result = _jonj_math_zvaltobase(&temp, tobase TSRMLS_CC);
	RETVAL_STRING(result, 0);
}
/* }}} */

/* {{{ _jonj_math_number_format
*/
JONJAPI char *_jonj_math_number_format(double d, int dec, char dec_point, char thousand_sep)
{
	return _jonj_math_number_format_ex(d, dec, &dec_point, 1, &thousand_sep, 1);
}

static char *_jonj_math_number_format_ex_len(double d, int dec, char *dec_point,
		size_t dec_point_len, char *thousand_sep, size_t thousand_sep_len,
		size_t *result_len)
{
	char *tmpbuf = NULL, *resbuf;
	char *s, *t;  /* source, target */
	char *dp;
	size_t integral;
	size_t tmplen, reslen=0;
	int count=0;
	int is_negative=0;

	if (d < 0) {
		is_negative = 1;
		d = -d;
	}

	dec = MAX(0, dec);
	d = _jonj_math_round(d, dec, JONJ_ROUND_HALF_UP);

	tmplen = spprintf(&tmpbuf, 0, "%.*F", dec, d);

	if (tmpbuf == NULL || !isdigit((int)tmpbuf[0])) {
		if (result_len) {
			*result_len = tmplen;
		}

		return tmpbuf;
	}

	/* find decimal point, if expected */
	if (dec) {
		dp = strpbrk(tmpbuf, ".,");
	} else {
		dp = NULL;
	}

	/* calculate the length of the return buffer */
	if (dp) {
		integral = dp - tmpbuf;
	} else {
		/* no decimal point was found */
		integral = tmplen;
	}

	/* allow for thousand separators */
	if (thousand_sep) {
		if (integral + thousand_sep_len * ((integral-1) / 3) < integral) {
			/* overflow */
			zend_error(E_ERROR, "String overflow");
		}
		integral += thousand_sep_len * ((integral-1) / 3);
	}

	reslen = integral;

	if (dec) {
		reslen += dec;

		if (dec_point) {
			if (reslen + dec_point < dec_point) {
				/* overflow */
				zend_error(E_ERROR, "String overflow");
			}
			reslen += dec_point_len;
		}
	}

	/* add a byte for minus sign */
	if (is_negative) {
		reslen++;
	}
	resbuf = (char *) emalloc(reslen+1); /* +1 for NUL terminator */

	s = tmpbuf+tmplen-1;
	t = resbuf+reslen;
	*t-- = '\0';

	/* copy the decimal places.
	 * Take care, as the sprintf implementation may return less places than
	 * we requested due to internal buffer limitations */
	if (dec) {
		int declen = dp ? s - dp : 0;
		int topad = dec > declen ? dec - declen : 0;

		/* pad with '0's */
		while (topad--) {
			*t-- = '0';
		}

		if (dp) {
			s -= declen + 1; /* +1 to skip the point */
			t -= declen;

			/* now copy the chars after the point */
			memcpy(t + 1, dp + 1, declen);
		}

		/* add decimal point */
		if (dec_point) {
			t -= dec_point_len;
			memcpy(t + 1, dec_point, dec_point_len);
		}
	}

	/* copy the numbers before the decimal point, adding thousand
	 * separator every three digits */
	while(s >= tmpbuf) {
		*t-- = *s--;
		if (thousand_sep && (++count%3)==0 && s>=tmpbuf) {
			t -= thousand_sep_len;
			memcpy(t + 1, thousand_sep, thousand_sep_len);
		}
	}

	/* and a minus sign, if needed */
	if (is_negative) {
		*t-- = '-';
	}

	efree(tmpbuf);

	if (result_len) {
		*result_len = reslen;
	}

	return resbuf;
}

JONJAPI char *_jonj_math_number_format_ex(double d, int dec, char *dec_point,
		size_t dec_point_len, char *thousand_sep, size_t thousand_sep_len)
{
	return _jonj_math_number_format_ex_len(d, dec, dec_point, dec_point_len,
			thousand_sep, thousand_sep_len, NULL);
}
/* }}} */

/* {{{ proto string number_format(float number [, int num_decimal_places [, string dec_separator, string thousands_separator]])
   Formats a number with grouped thousands */
JONJ_FUNCTION(number_format)
{
	double num;
	long dec = 0;
	char *thousand_sep = NULL, *dec_point = NULL;
	char thousand_sep_chr = ',', dec_point_chr = '.';
	int thousand_sep_len = 0, dec_point_len = 0;
	char *formatted;
	size_t formatted_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d|ls!s!", &num, &dec, &dec_point, &dec_point_len, &thousand_sep, &thousand_sep_len) == FAILURE) {
		return;
	}

	switch(ZEND_NUM_ARGS()) {
	case 1:
		formatted = _jonj_math_number_format(num, 0, dec_point_chr, thousand_sep_chr);
		formatted_len = strlen(formatted);
		break;
	case 2:
		formatted = _jonj_math_number_format(num, dec, dec_point_chr, thousand_sep_chr);
		formatted_len = strlen(formatted);
		break;
	case 4:
		if (dec_point == NULL) {
			dec_point = &dec_point_chr;
			dec_point_len = 1;
		}

		if (thousand_sep == NULL) {
			thousand_sep = &thousand_sep_chr;
			thousand_sep_len = 1;
		}

		formatted = _jonj_math_number_format_ex_len(num, dec,
				dec_point, dec_point_len, thousand_sep, thousand_sep_len,
				&formatted_len);
		break;
	default:
		WRONG_PARAM_COUNT;
		return;
	}
	RETVAL_STRINGL_CHECK(formatted, formatted_len, 0);
}
/* }}} */

/* {{{ proto float fmod(float x, float y)
   Returns the remainder of dividing x by y as a float */
JONJ_FUNCTION(fmod)
{
	double num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd",  &num1, &num2) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(fmod(num1, num2));
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
