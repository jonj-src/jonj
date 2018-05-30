dnl $Id$
dnl config.m4 for extension ereg

dnl
dnl Check for regex library type
dnl
JONJ_ARG_WITH(regex,,
[  --with-regex=TYPE       Regex library type: system, jonj. [TYPE=jonj]
                          WARNING: Do NOT use unless you know what you are doing!], jonj, no)

case $JONJ_REGEX in
  system)
    if test "$JONJ_SAPI" = "apache" || test "$JONJ_SAPI" = "apache2filter" || test "$JONJ_SAPI" = "apache2handler"; then
      REGEX_TYPE=jonj
    else
      REGEX_TYPE=system
    fi
    ;;
  yes | jonj)
    REGEX_TYPE=jonj
    ;;
  *)
    REGEX_TYPE=jonj
    AC_MSG_WARN([Invalid regex library type selected. Using default value: jonj])
    ;;
esac

AC_MSG_CHECKING([which regex library to use])
AC_MSG_RESULT([$REGEX_TYPE])

if test "$REGEX_TYPE" = "jonj"; then
  ereg_regex_sources="regex/regcomp.c regex/regexec.c regex/regerror.c regex/regfree.c"
  ereg_regex_headers="regex/"
  JONJ_EREG_CFLAGS="-Dregexec=jonj_regexec -Dregerror=jonj_regerror -Dregfree=jonj_regfree -Dregcomp=jonj_regcomp"
fi

JONJ_NEW_EXTENSION(ereg, ereg.c $ereg_regex_sources, no,,$JONJ_EREG_CFLAGS)
JONJ_INSTALL_HEADERS([ext/ereg], [jonj_ereg.h jonj_regex.h $ereg_regex_headers])

if test "$REGEX_TYPE" = "jonj"; then
  AC_DEFINE(HAVE_REGEX_T_RE_MAGIC, 1, [ ])
  AC_DEFINE(HSREGEX,1,[ ])
  AC_DEFINE(REGEX,1,[ ])  
  JONJ_ADD_BUILD_DIR([$ext_builddir/regex], 1)
  JONJ_ADD_INCLUDE([$ext_srcdir/regex])
elif test "$REGEX_TYPE" = "system"; then
  AC_DEFINE(REGEX,0,[ ])
  dnl Check if field re_magic exists in struct regex_t
  AC_CACHE_CHECK([whether field re_magic exists in struct regex_t], ac_cv_regex_t_re_magic, [
  AC_TRY_COMPILE([#include <sys/types.h>
#include <regex.h>], [regex_t rt; rt.re_magic;],
  [ac_cv_regex_t_re_magic=yes], [ac_cv_regex_t_re_magic=no])])
  if test "$ac_cv_regex_t_re_magic" = "yes"; then
    AC_DEFINE([HAVE_REGEX_T_RE_MAGIC], [ ], 1)   
  fi 
fi   
