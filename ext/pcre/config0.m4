dnl
dnl $Id$
dnl

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

JONJ_ARG_WITH(pcre-regex,,
[  --with-pcre-regex=DIR   Include Perl Compatible Regular Expressions support.
                          DIR is the PCRE install prefix [BUNDLED]], yes, no)

  if test "$JONJ_PCRE_REGEX" != "yes" && test "$JONJ_PCRE_REGEX" != "no"; then
    AC_MSG_CHECKING([for PCRE headers location])
    for i in $JONJ_PCRE_REGEX $JONJ_PCRE_REGEX/include $JONJ_PCRE_REGEX/include/pcre $JONJ_PCRE_REGEX/local/include; do
      test -f $i/pcre.h && PCRE_INCDIR=$i
    done

    if test -z "$PCRE_INCDIR"; then
      AC_MSG_ERROR([Could not find pcre.h in $JONJ_PCRE_REGEX])
    fi
    AC_MSG_RESULT([$PCRE_INCDIR])

    AC_MSG_CHECKING([for PCRE library location])
    for j in $JONJ_PCRE_REGEX $JONJ_PCRE_REGEX/$JONJ_LIBDIR; do
      test -f $j/libpcre.a || test -f $j/libpcre.$SHLIB_SUFFIX_NAME && PCRE_LIBDIR=$j
    done
    
    if test -z "$PCRE_LIBDIR" ; then
      AC_MSG_ERROR([Could not find libpcre.(a|$SHLIB_SUFFIX_NAME) in $JONJ_PCRE_REGEX])
    fi
    AC_MSG_RESULT([$PCRE_LIBDIR])

    changequote({,})
    pcre_major=`grep PCRE_MAJOR $PCRE_INCDIR/pcre.h | sed -e 's/[^0-9]//g'`
    pcre_minor=`grep PCRE_MINOR $PCRE_INCDIR/pcre.h | sed -e 's/[^0-9]//g'`
    changequote([,])
    pcre_minor_length=`echo "$pcre_minor" | wc -c | sed -e 's/[^0-9]//g'`
    if test "$pcre_minor_length" -eq 2 ; then
      pcre_minor="$pcre_minor"0
    fi
    pcre_version=$pcre_major$pcre_minor
    if test "$pcre_version" -lt 660; then
      AC_MSG_ERROR([The PCRE extension requires PCRE library version >= 6.6])
    fi

    JONJ_ADD_LIBRARY_WITH_PATH(pcre, $PCRE_LIBDIR)
    
    AC_DEFINE(HAVE_PCRE, 1, [ ])
    JONJ_ADD_INCLUDE($PCRE_INCDIR)
    JONJ_NEW_EXTENSION(pcre, jonj_pcre.c, no)
    JONJ_INSTALL_HEADERS([ext/pcre], [jonj_pcre.h])
  else
    AC_MSG_CHECKING([for PCRE library to use])
    AC_MSG_RESULT([bundled])
    pcrelib_sources="pcrelib/pcre_chartables.c pcrelib/pcre_ucd.c \
    				 pcrelib/pcre_compile.c pcrelib/pcre_config.c pcrelib/pcre_exec.c \
    				 pcrelib/pcre_fullinfo.c pcrelib/pcre_get.c pcrelib/pcre_globals.c \
    				 pcrelib/pcre_maketables.c pcrelib/pcre_newline.c \
    				 pcrelib/pcre_ord2utf8.c pcrelib/pcre_refcount.c pcrelib/pcre_study.c \
    				 pcrelib/pcre_tables.c pcrelib/pcre_valid_utf8.c \
    				 pcrelib/pcre_version.c pcrelib/pcre_xclass.c \
    				 pcrelib/pcre_jit_compile.c"
    JONJ_PCRE_CFLAGS="-DHAVE_CONFIG_H -I@ext_srcdir@/pcrelib"
    JONJ_NEW_EXTENSION(pcre, $pcrelib_sources jonj_pcre.c, no,,$JONJ_PCRE_CFLAGS)
    JONJ_ADD_BUILD_DIR($ext_builddir/pcrelib)
    JONJ_INSTALL_HEADERS([ext/pcre], [jonj_pcre.h pcrelib/])
    AC_DEFINE(HAVE_BUNDLED_PCRE, 1, [ ])
  fi
