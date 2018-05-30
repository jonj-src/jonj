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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef JONJ_H
#define JONJ_H

#ifdef HAVE_DMALLOC
#include <dmalloc.h>
#endif

#define JONJ_API_VERSION 20131106
#define JONJ_HAVE_STREAMS
#define YYDEBUG 0
#define JONJ_DEFAULT_CHARSET "UTF-8"

#include "jonj_version.h"
#include "zend.h"
#include "zend_qsort.h"
#include "jonj_compat.h"

#include "zend_API.h"

#undef sprintf
#define sprintf jonj_sprintf

/* JONJ's DEBUG value must match Zend's ZEND_DEBUG value */
#undef JONJ_DEBUG
#define JONJ_DEBUG ZEND_DEBUG

#ifdef JONJ_WIN32
#	include "tsrm_win32.h"
#	include "win95nt.h"
#	ifdef JONJ_EXPORTS
#		define JONJAPI __declspec(dllexport)
#	else
#		define JONJAPI __declspec(dllimport)
#	endif
#	define JONJ_DIR_SEPARATOR '\\'
#	define JONJ_EOL "\r\n"
#else
#	if defined(__GNUC__) && __GNUC__ >= 4
#		define JONJAPI __attribute__ ((visibility("default")))
#	else
#		define JONJAPI
#	endif
#	define THREAD_LS
#	define JONJ_DIR_SEPARATOR '/'
#	define JONJ_EOL "\n"
#endif

#ifdef NETWARE
/* For jonj_get_uname() function */
#define JONJ_UNAME  "NetWare"
#define JONJ_OS      JONJ_UNAME
#endif

#if HAVE_ASSERT_H
#if JONJ_DEBUG
#undef NDEBUG
#else
#ifndef NDEBUG
#define NDEBUG
#endif
#endif
#include <assert.h>
#else /* HAVE_ASSERT_H */
#define assert(expr) ((void) (0))
#endif /* HAVE_ASSERT_H */

#define APACHE 0

#if HAVE_UNIX_H
#include <unix.h>
#endif

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#if HAVE_BUILD_DEFS_H
#include <build-defs.h>
#endif

/*
 * This is a fast version of strlcpy which should be used, if you
 * know the size of the destination buffer and if you know
 * the length of the source string.
 *
 * size is the allocated number of bytes of dst
 * src_size is the number of bytes excluding the NUL of src
 */

#define JONJ_STRLCPY(dst, src, size, src_size)	\
	{											\
		size_t jonj_str_len;						\
												\
		if (src_size >= size)					\
			jonj_str_len = size - 1;				\
		else									\
			jonj_str_len = src_size;				\
		memcpy(dst, src, jonj_str_len);			\
		dst[jonj_str_len] = '\0';				\
	}

#ifndef HAVE_STRLCPY
BEGIN_EXTERN_C()
JONJAPI size_t jonj_strlcpy(char *dst, const char *src, size_t siz);
END_EXTERN_C()
#undef strlcpy
#define strlcpy jonj_strlcpy
#endif

#ifndef HAVE_STRLCAT
BEGIN_EXTERN_C()
JONJAPI size_t jonj_strlcat(char *dst, const char *src, size_t siz);
END_EXTERN_C()
#undef strlcat
#define strlcat jonj_strlcat
#endif

#ifndef HAVE_STRTOK_R
BEGIN_EXTERN_C()
char *strtok_r(char *s, const char *delim, char **last);
END_EXTERN_C()
#endif

#ifndef HAVE_SOCKLEN_T
# if JONJ_WIN32
typedef int socklen_t;
# else
typedef unsigned int socklen_t;
# endif
#endif

#define CREATE_MUTEX(a, b)
#define SET_MUTEX(a)
#define FREE_MUTEX(a)

/*
 * Then the ODBC support can use both iodbc and Solid,
 * uncomment this.
 * #define HAVE_ODBC (HAVE_IODBC|HAVE_SOLID)
 */

#include <stdlib.h>
#include <ctype.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STDARG_H
#include <stdarg.h>
#else
# if HAVE_SYS_VARARGS_H
# include <sys/varargs.h>
# endif
#endif

#ifndef va_copy
# ifdef __va_copy
#  define va_copy(ap1, ap2)         __va_copy((ap1), (ap2))
# else
#  define va_copy(ap1, ap2)         memcpy((&ap1), (&ap2), sizeof(va_list))
# endif
#endif

#include "jonj_stdint.h"

#include "zend_hash.h"
#include "zend_alloc.h"
#include "zend_stack.h"

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
# endif
# ifndef HAVE_MEMMOVE
#  define memmove(d, s, n)	bcopy ((s), (d), (n))
# endif
#endif

#ifndef HAVE_STRERROR
char *strerror(int);
#endif

#if HAVE_PWD_H
# ifdef JONJ_WIN32
#include "win32/param.h"
# else
#include <pwd.h>
#include <sys/param.h>
# endif
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef INT_MIN
#define INT_MIN (- INT_MAX - 1)
#endif

#define JONJ_GCC_VERSION ZEND_GCC_VERSION
#define JONJ_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_MALLOC
#define JONJ_ATTRIBUTE_FORMAT ZEND_ATTRIBUTE_FORMAT

BEGIN_EXTERN_C()
#include "snprintf.h"
END_EXTERN_C()
#include "spprintf.h"

#define EXEC_INPUT_BUF 4096

#define JONJ_MIME_TYPE "application/x-httpd-jonj"

/* macros */
#define STR_PRINT(str)	((str)?(str):"")

#ifndef MAXPATHLEN
# ifdef PATH_MAX
#  define MAXPATHLEN PATH_MAX
# elif defined(MAX_PATH)
#  define MAXPATHLEN MAX_PATH
# else
#  define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
# endif
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
# define jonj_ignore_value(x) (({ __typeof__ (x) __x = (x); (void) __x; }))
#else
# define jonj_ignore_value(x) ((void) (x))
#endif

/* global variables */
#if !defined(JONJ_WIN32)
#define JONJ_SLEEP_NON_VOID
#define jonj_sleep sleep
extern char **environ;
#endif	/* !defined(JONJ_WIN32) */

#ifdef JONJ_PWRITE_64
ssize_t pwrite(int, void *, size_t, off64_t);
#endif

#ifdef JONJ_PREAD_64
ssize_t pread(int, void *, size_t, off64_t);
#endif

BEGIN_EXTERN_C()
void jonjerror(char *error);
JONJAPI int jonj_write(void *buf, uint size TSRMLS_DC);
JONJAPI int jonj_printf(const char *format, ...) JONJ_ATTRIBUTE_FORMAT(printf, 1,
		2);
JONJAPI int jonj_get_module_initialized(void);
JONJAPI void jonj_log_err(char *log_message TSRMLS_DC);
int Debug(char *format, ...) JONJ_ATTRIBUTE_FORMAT(printf, 1, 2);
int cfgparse(void);
END_EXTERN_C()

#define jonj_error zend_error
#define error_handling_t zend_error_handling_t

BEGIN_EXTERN_C()
static inline ZEND_ATTRIBUTE_DEPRECATED void jonj_set_error_handling(error_handling_t error_handling, zend_class_entry *exception_class TSRMLS_DC)
{
	zend_replace_error_handling(error_handling, exception_class, NULL TSRMLS_CC);
}
static inline ZEND_ATTRIBUTE_DEPRECATED void jonj_std_error_handling() {}

JONJAPI void jonj_verror(const char *docref, const char *params, int type, const char *format, va_list args TSRMLS_DC) JONJ_ATTRIBUTE_FORMAT(printf, 4, 0);

#ifdef ZTS
#define JONJ_ATTR_FMT_OFFSET 1
#else
#define JONJ_ATTR_FMT_OFFSET 0
#endif

/* JONJAPI void jonj_error(int type, const char *format, ...); */
JONJAPI void jonj_error_docref0(const char *docref TSRMLS_DC, int type, const char *format, ...)
	JONJ_ATTRIBUTE_FORMAT(printf, JONJ_ATTR_FMT_OFFSET + 3, JONJ_ATTR_FMT_OFFSET + 4);
JONJAPI void jonj_error_docref1(const char *docref TSRMLS_DC, const char *param1, int type, const char *format, ...)
	JONJ_ATTRIBUTE_FORMAT(printf, JONJ_ATTR_FMT_OFFSET + 4, JONJ_ATTR_FMT_OFFSET + 5);
JONJAPI void jonj_error_docref2(const char *docref TSRMLS_DC, const char *param1, const char *param2, int type, const char *format, ...)
	JONJ_ATTRIBUTE_FORMAT(printf, JONJ_ATTR_FMT_OFFSET + 5, JONJ_ATTR_FMT_OFFSET + 6);
#ifdef JONJ_WIN32
JONJAPI void jonj_win32_docref2_from_error(DWORD error, const char *param1, const char *param2 TSRMLS_DC);
#endif
END_EXTERN_C()

#define jonj_error_docref jonj_error_docref0

#define zenderror jonjerror
#define zendlex jonjlex

#define jonjparse zendparse
#define jonjrestart zendrestart
#define jonjin zendin

#define jonj_memnstr zend_memnstr

/* functions */
BEGIN_EXTERN_C()
JONJAPI extern int (*jonj_register_internal_extensions_func)(TSRMLS_D);
JONJAPI int jonj_register_internal_extensions(TSRMLS_D);
JONJAPI int jonj_mergesort(void *base, size_t nmemb, register size_t size, int (*cmp)(const void *, const void * TSRMLS_DC) TSRMLS_DC);
JONJAPI void jonj_register_pre_request_shutdown(void (*func)(void *), void *userdata);
JONJAPI void jonj_com_initialize(TSRMLS_D);
JONJAPI char *jonj_get_current_user(TSRMLS_D);
END_EXTERN_C()

/* JONJ-named Zend macro wrappers */
#define JONJ_FN					ZEND_FN
#define JONJ_MN					ZEND_MN
#define JONJ_NAMED_FUNCTION		ZEND_NAMED_FUNCTION
#define JONJ_FUNCTION			ZEND_FUNCTION
#define JONJ_METHOD  			ZEND_METHOD

#define JONJ_RAW_NAMED_FE ZEND_RAW_NAMED_FE
#define JONJ_NAMED_FE	ZEND_NAMED_FE
#define JONJ_FE			ZEND_FE
#define JONJ_DEP_FE      ZEND_DEP_FE
#define JONJ_FALIAS		ZEND_FALIAS
#define JONJ_DEP_FALIAS	ZEND_DEP_FALIAS
#define JONJ_ME          ZEND_ME
#define JONJ_MALIAS      ZEND_MALIAS
#define JONJ_ABSTRACT_ME ZEND_ABSTRACT_ME
#define JONJ_ME_MAPPING  ZEND_ME_MAPPING
#define JONJ_FE_END      ZEND_FE_END

#define JONJ_MODULE_STARTUP_N	ZEND_MODULE_STARTUP_N
#define JONJ_MODULE_SHUTDOWN_N	ZEND_MODULE_SHUTDOWN_N
#define JONJ_MODULE_ACTIVATE_N	ZEND_MODULE_ACTIVATE_N
#define JONJ_MODULE_DEACTIVATE_N	ZEND_MODULE_DEACTIVATE_N
#define JONJ_MODULE_INFO_N		ZEND_MODULE_INFO_N

#define JONJ_MODULE_STARTUP_D	ZEND_MODULE_STARTUP_D
#define JONJ_MODULE_SHUTDOWN_D	ZEND_MODULE_SHUTDOWN_D
#define JONJ_MODULE_ACTIVATE_D	ZEND_MODULE_ACTIVATE_D
#define JONJ_MODULE_DEACTIVATE_D	ZEND_MODULE_DEACTIVATE_D
#define JONJ_MODULE_INFO_D		ZEND_MODULE_INFO_D

/* Compatibility macros */
#define JONJ_MINIT		ZEND_MODULE_STARTUP_N
#define JONJ_MSHUTDOWN	ZEND_MODULE_SHUTDOWN_N
#define JONJ_RINIT		ZEND_MODULE_ACTIVATE_N
#define JONJ_RSHUTDOWN	ZEND_MODULE_DEACTIVATE_N
#define JONJ_MINFO		ZEND_MODULE_INFO_N
#define JONJ_GINIT		ZEND_GINIT
#define JONJ_GSHUTDOWN	ZEND_GSHUTDOWN

#define JONJ_MINIT_FUNCTION		ZEND_MODULE_STARTUP_D
#define JONJ_MSHUTDOWN_FUNCTION	ZEND_MODULE_SHUTDOWN_D
#define JONJ_RINIT_FUNCTION		ZEND_MODULE_ACTIVATE_D
#define JONJ_RSHUTDOWN_FUNCTION	ZEND_MODULE_DEACTIVATE_D
#define JONJ_MINFO_FUNCTION		ZEND_MODULE_INFO_D
#define JONJ_GINIT_FUNCTION		ZEND_GINIT_FUNCTION
#define JONJ_GSHUTDOWN_FUNCTION	ZEND_GSHUTDOWN_FUNCTION
 
#define JONJ_MODULE_GLOBALS		ZEND_MODULE_GLOBALS


/* Output support */
#include "main/jonj_output.h"


#include "jonj_streams.h"
#include "jonj_memory_streams.h"
#include "fopen_wrappers.h"


/* Virtual current working directory support */
#include "zend_virtual_cwd.h"

#include "zend_constants.h"

/* connection status states */
#define JONJ_CONNECTION_NORMAL  0
#define JONJ_CONNECTION_ABORTED 1
#define JONJ_CONNECTION_TIMEOUT 2

#include "jonj_reentrancy.h"

/* Finding offsets of elements within structures.
 * Taken from the Apache code, which in turn, was taken from X code...
 */

#ifndef XtOffset
#if defined(CRAY) || (defined(__arm) && !(defined(LINUX) || defined(__riscos__)))
#ifdef __STDC__
#define XtOffset(p_type, field) _Offsetof(p_type, field)
#else
#ifdef CRAY2
#define XtOffset(p_type, field) \
    (sizeof(int)*((unsigned int)&(((p_type)NULL)->field)))

#else /* !CRAY2 */

#define XtOffset(p_type, field) ((unsigned int)&(((p_type)NULL)->field))

#endif /* !CRAY2 */
#endif /* __STDC__ */
#else /* ! (CRAY || __arm) */

#define XtOffset(p_type, field) \
    ((long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#endif /* !CRAY */
#endif /* ! XtOffset */

#ifndef XtOffsetOf
#ifdef offsetof
#define XtOffsetOf(s_type, field) offsetof(s_type, field)
#else
#define XtOffsetOf(s_type, field) XtOffset(s_type*, field)
#endif
#endif /* !XtOffsetOf */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
