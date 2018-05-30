dnl $Id$
dnl config.m4 for input filtering extension

JONJ_ARG_ENABLE(filter, whether to enable input filter support,
[  --disable-filter        Disable input filter support], yes)

JONJ_ARG_WITH(pcre-dir, pcre install prefix,
[  --with-pcre-dir         FILTER: pcre install prefix], no, no)

if test "$JONJ_FILTER" != "no"; then

  dnl Check if configure is the JONJ core configure
  if test -n "$JONJ_VERSION"; then
    dnl This extension can not be build as shared when in JONJ core
    ext_shared=no
  else
    dnl This is PECL build, check if bundled PCRE library is used
    old_CPPFLAGS=$CPPFLAGS
    CPPFLAGS=$INCLUDES
    AC_EGREP_CPP(yes,[
#include <main/jonj_config.h>
#if defined(HAVE_BUNDLED_PCRE) && !defined(COMPILE_DL_PCRE)
yes
#endif
    ],[
      JONJ_PCRE_REGEX=yes
    ],[
      AC_EGREP_CPP(yes,[
#include <main/jonj_config.h>
#if defined(HAVE_PCRE) && !defined(COMPILE_DL_PCRE)
yes
#endif
      ],[
        JONJ_PCRE_REGEX=pecl
      ],[
        JONJ_PCRE_REGEX=no
      ])
    ])
    CPPFLAGS=$old_CPPFLAGS
  fi

  JONJ_NEW_EXTENSION(filter, filter.c sanitizing_filters.c logical_filters.c callback_filter.c, $ext_shared)
  JONJ_SUBST(FILTER_SHARED_LIBADD)

  JONJ_INSTALL_HEADERS([ext/filter/jonj_filter.h])
  JONJ_ADD_EXTENSION_DEP(filter, pcre)
fi
