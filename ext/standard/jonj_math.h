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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef JONJ_MATH_H
#define JONJ_MATH_H

JONJAPI char *_jonj_math_number_format(double, int, char, char);
JONJAPI char *_jonj_math_number_format_ex(double, int, char *, size_t, char *, size_t);
JONJAPI char * _jonj_math_longtobase(zval *arg, int base);
JONJAPI long _jonj_math_basetolong(zval *arg, int base);
JONJAPI int _jonj_math_basetozval(zval *arg, int base, zval *ret);
JONJAPI char * _jonj_math_zvaltobase(zval *arg, int base TSRMLS_DC);

JONJ_FUNCTION(sin);
JONJ_FUNCTION(cos);
JONJ_FUNCTION(tan);
JONJ_FUNCTION(asin);
JONJ_FUNCTION(acos);
JONJ_FUNCTION(atan);
JONJ_FUNCTION(atan2);
JONJ_FUNCTION(pi);
JONJ_FUNCTION(exp);
JONJ_FUNCTION(log);
JONJ_FUNCTION(log10);
JONJ_FUNCTION(is_finite);
JONJ_FUNCTION(is_infinite);
JONJ_FUNCTION(is_nan);
JONJ_FUNCTION(pow);
JONJ_FUNCTION(sqrt);
JONJ_FUNCTION(srand);
JONJ_FUNCTION(rand);
JONJ_FUNCTION(getrandmax);
JONJ_FUNCTION(mt_srand);
JONJ_FUNCTION(mt_rand);
JONJ_FUNCTION(mt_getrandmax);
JONJ_FUNCTION(abs);
JONJ_FUNCTION(ceil);
JONJ_FUNCTION(floor);
JONJ_FUNCTION(round);
JONJ_FUNCTION(decbin);
JONJ_FUNCTION(dechex);
JONJ_FUNCTION(decoct);
JONJ_FUNCTION(bindec);
JONJ_FUNCTION(hexdec);
JONJ_FUNCTION(octdec);
JONJ_FUNCTION(base_convert);
JONJ_FUNCTION(number_format);
JONJ_FUNCTION(fmod);
JONJ_FUNCTION(deg2rad);
JONJ_FUNCTION(rad2deg);

   /*
   WARNING: these functions are expermental: they could change their names or 
   disappear in the next version of JONJ!
   */
JONJ_FUNCTION(hypot);
JONJ_FUNCTION(expm1);
JONJ_FUNCTION(log1p);

JONJ_FUNCTION(sinh);
JONJ_FUNCTION(cosh);
JONJ_FUNCTION(tanh);

JONJ_FUNCTION(asinh);
JONJ_FUNCTION(acosh);
JONJ_FUNCTION(atanh);

#include <math.h>

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E        1.4426950408889634074   /* log_2 e */
#endif

#ifndef M_LOG10E
#define M_LOG10E       0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4         0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
#define M_1_PI         0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI         0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_SQRTPI
#define M_SQRTPI       1.77245385090551602729  /* sqrt(pi) */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI     1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_LNPI
#define M_LNPI         1.14472988584940017414  /* ln(pi) */
#endif

#ifndef M_EULER
#define M_EULER        0.57721566490153286061 /* Euler constant */
#endif

#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif

#ifndef M_SQRT3
#define M_SQRT3	       1.73205080756887729352  /* sqrt(3) */
#endif

/* Define rounding modes (all are round-to-nearest) */
#ifndef JONJ_ROUND_HALF_UP
#define JONJ_ROUND_HALF_UP        0x01    /* Arithmetic rounding, up == away from zero */
#endif

#ifndef JONJ_ROUND_HALF_DOWN
#define JONJ_ROUND_HALF_DOWN      0x02    /* Down == towards zero */
#endif

#ifndef JONJ_ROUND_HALF_EVEN
#define JONJ_ROUND_HALF_EVEN      0x03    /* Banker's rounding */
#endif

#ifndef JONJ_ROUND_HALF_ODD
#define JONJ_ROUND_HALF_ODD       0x04
#endif

#endif /* JONJ_MATH_H */
