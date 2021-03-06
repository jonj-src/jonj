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
   | Author: Stig Sæther Bakken <ssb@jonj.tk>                             |
   |         Marcus Boerger <helly@jonj.tk>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*

Comparing: sprintf, snprintf, slprintf, spprintf

sprintf  offers the ability to make a lot of failures since it does not know
         the size of the buffer it uses. Therefore usage of sprintf often
         results in possible entries for buffer overrun attacks. So please
         use this version only if you are sure the call is safe. sprintf
         always terminstes the buffer it writes to.

snprintf knows the buffers size and will not write behind it. But you will
         have to use either a static buffer or allocate a dynamic buffer
         before beeing able to call the function. In other words you must
         be sure that you really know the maximum size of the buffer required.
         A bad thing is having a big maximum while in most cases you would
         only need a small buffer. If the size of the resulting string is
         longer or equal to the buffer size than the buffer is not terminated.
         The function also returns the number of chars not including the
         terminating \0 that were needed to fully comply to the print request.

slprintf same as snprintf with the difference that it actually returns the
         length printed not including the terminating \0.

spprintf is the dynamical version of snprintf. It allocates the buffer in size
         as needed and allows a maximum setting as snprintf (turn this feature
         off by setting max_len to 0). spprintf is a little bit slower than
         snprintf and offers possible memory leakes if you miss freeing the
         buffer allocated by the function. Therfore this function should be
         used where either no maximum is known or the maximum is much bigger
         than normal size required. spprintf always terminates the buffer.

Example:

 #define MAX 1024              | #define MAX 1024               | #define MAX 1024
 char buffer[MAX]              | char buffer[MAX]               | char *buffer;
                               |                                |
                               |                                | // No need to initialize buffer:
                               |                                | // spprintf ignores value of buffer
 sprintf(buffer, "test");      | snprintf(buffer, MAX, "test"); | spprintf(&buffer, MAX, "text");
                               |                                | if (!buffer)
                               |                                |   return OUT_OF_MEMORY
 // sprintf always terminates | // manual termination of       | // spprintf allays terminates buffer
 // buffer                     | // buffer *IS* required        |
                               | buffer[MAX-1] = 0;             |
 action_with_buffer(buffer);   | action_with_buffer(buffer);    | action_with_buffer(buffer);
                               |                                | efree(buffer);
*/

#ifndef SNPRINTF_H
#define SNPRINTF_H

typedef int bool_int;

typedef enum {
	NO = 0, YES = 1
} boolean_e;


BEGIN_EXTERN_C()
JONJAPI int ap_jonj_slprintf(char *buf, size_t len, const char *format,...);
JONJAPI int ap_jonj_vslprintf(char *buf, size_t len, const char *format, va_list ap);
JONJAPI int ap_jonj_snprintf(char *, size_t, const char *, ...);
JONJAPI int ap_jonj_vsnprintf(char *, size_t, const char *, va_list ap);
JONJAPI int ap_jonj_vasprintf(char **buf, const char *format, va_list ap);
JONJAPI int ap_jonj_asprintf(char **buf, const char *format, ...);
JONJAPI int jonj_sprintf (char* s, const char* format, ...) JONJ_ATTRIBUTE_FORMAT(printf, 2, 3);
JONJAPI char * jonj_gcvt(double value, int ndigit, char dec_point, char exponent, char *buf);
JONJAPI char * jonj_conv_fp(register char format, register double num,
		 boolean_e add_dp, int precision, char dec_point, bool_int * is_negative, char *buf, int *len);

END_EXTERN_C()

#ifdef slprintf
#undef slprintf
#endif
#define slprintf ap_jonj_slprintf

#ifdef vslprintf
#undef vslprintf
#endif
#define vslprintf ap_jonj_vslprintf

#ifdef snprintf
#undef snprintf
#endif
#define snprintf ap_jonj_snprintf

#ifdef vsnprintf
#undef vsnprintf
#endif
#define vsnprintf ap_jonj_vsnprintf

#ifndef HAVE_VASPRINTF
#define vasprintf ap_jonj_vasprintf
#endif

#ifndef HAVE_ASPRINTF
#define asprintf ap_jonj_asprintf
#endif

#ifdef sprintf
#undef sprintf
#endif
#define sprintf jonj_sprintf

typedef enum {
	LM_STD = 0,
#if SIZEOF_INTMAX_T
	LM_INTMAX_T,
#endif
#if SIZEOF_PTRDIFF_T
	LM_PTRDIFF_T,
#endif
#if SIZEOF_LONG_LONG
	LM_LONG_LONG,
#endif
	LM_SIZE_T,
	LM_LONG,
	LM_LONG_DOUBLE
} length_modifier_e;

#ifdef JONJ_WIN32
# define WIDE_INT		__int64
#elif SIZEOF_LONG_LONG_INT
# define WIDE_INT		long long int
#elif SIZEOF_LONG_LONG
# define WIDE_INT		long long
#else
# define WIDE_INT		long
#endif
typedef WIDE_INT wide_int;
typedef unsigned WIDE_INT u_wide_int;

JONJAPI char * ap_jonj_conv_10(register wide_int num, register bool_int is_unsigned,
	   register bool_int * is_negative, char *buf_end, register int *len);

JONJAPI char * ap_jonj_conv_p2(register u_wide_int num, register int nbits,
		 char format, char *buf_end, register int *len);

/* The maximum precision that's allowed for float conversion. Does not include
 * decimal separator, exponent, sign, terminator. Currently does not affect
 * the modes e/f, only g/k/H, as those have a different limit enforced at
 * another level (see NDIG in jonj_conv_fp()).
 * Applies to the formatting functions of both spprintf.c and snprintf.c, which
 * use equally sized buffers of MAX_BUF_SIZE = 512 to hold the result of the
 * call to jonj_gcvt().
 * This should be reasonably smaller than MAX_BUF_SIZE (I think MAX_BUF_SIZE - 9
 * should be enough, but let's give some more space) */
#define FORMAT_CONV_MAX_PRECISION 500

#endif /* SNPRINTF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
