dnl
dnl $Id$
dnl

JONJ_ARG_WITH(iconv, for iconv support,
[  --without-iconv[=DIR]     Exclude iconv support], yes)

if test "$JONJ_ICONV" != "no"; then

  JONJ_SETUP_ICONV(ICONV_SHARED_LIBADD, [
    iconv_avail="yes";
  ],[
    iconv_avail="no";
  ])

  if test "$iconv_avail" != "no"; then
    if test -z "$ICONV_DIR"; then
      for i in /usr/local /usr; do
        if test -f "$i/include/iconv.h" || test -f "$i/include/giconv.h"; then
          JONJ_ICONV_PREFIX="$i"
          break
        fi
      done
      if test -z "$JONJ_ICONV_PREFIX"; then
        JONJ_ICONV_PREFIX="/usr"
      fi
    else
      JONJ_ICONV_PREFIX="$ICONV_DIR"
    fi

    CFLAGS="-I$JONJ_ICONV_PREFIX/include $CFLAGS"
    LDFLAGS="-L$JONJ_ICONV_PREFIX/$JONJ_LIBDIR $LDFLAGS"

    if test -r "$JONJ_ICONV_PREFIX/include/giconv.h"; then
      JONJ_ICONV_H_PATH="$JONJ_ICONV_PREFIX/include/giconv.h"
    else
      JONJ_ICONV_H_PATH="$JONJ_ICONV_PREFIX/include/iconv.h"
	fi

    AC_MSG_CHECKING([if iconv is glibc's])
    AC_TRY_LINK([#include <gnu/libc-version.h>],[gnu_get_libc_version();],
    [
      AC_MSG_RESULT(yes)
      iconv_impl_name="glibc"
    ],[
      AC_MSG_RESULT(no)
    ])

    if test -z "$iconv_impl_name"; then
      AC_MSG_CHECKING([if using GNU libiconv])
      jonj_iconv_old_ld="$LDFLAGS"
      LDFLAGS="-liconv $LDFLAGS"
      AC_TRY_RUN([
#include <$JONJ_ICONV_H_PATH>
int main() {
  printf("%d", _libiconv_version);
  return 0;
}
      ],[
        AC_MSG_RESULT(yes)
        iconv_impl_name="gnu_libiconv"
      ],[
        AC_MSG_RESULT(no)
        LDFLAGS="$jonj_iconv_old_ld"
      ],[
        AC_MSG_RESULT(no, cross-compiling)
        LDFLAGS="$jonj_iconv_old_ld"
      ])
    fi

    if test -z "$iconv_impl_name"; then
      AC_MSG_CHECKING([if iconv is Konstantin Chuguev's])
      AC_TRY_LINK([#include <iconv.h>],[iconv_ccs_init(NULL, NULL);],
      [
        AC_MSG_RESULT(yes)
        iconv_impl_name="bsd"
      ],[
        AC_MSG_RESULT(no)
      ])
    fi

    if test -z "$iconv_impl_name"; then
      AC_MSG_CHECKING([if using IBM iconv])
      jonj_iconv_old_ld="$LDFLAGS"
      LDFLAGS="-liconv $LDFLAGS"
      AC_TRY_LINK([#include <iconv.h>],[cstoccsid("");],
      [
        AC_MSG_RESULT(yes)
        iconv_impl_name="ibm"
      ],[
        AC_MSG_RESULT(no)
        LDFLAGS="$jonj_iconv_old_ld"
      ])
    fi

    echo > ext/iconv/jonj_have_bsd_iconv.h
    echo > ext/iconv/jonj_have_glibc_iconv.h
    echo > ext/iconv/jonj_have_libiconv.h
    echo > ext/iconv/jonj_have_ibm_iconv.h

    case "$iconv_impl_name" in
      gnu_libiconv [)]
        JONJ_DEFINE([JONJ_ICONV_IMPL],[\"libiconv\"],[ext/iconv])
        AC_DEFINE([JONJ_ICONV_IMPL],["libiconv"],[Which iconv implementation to use])
        JONJ_DEFINE([HAVE_LIBICONV],1,[ext/iconv])
        JONJ_ADD_LIBRARY_WITH_PATH(iconv, "$JONJ_ICONV_PREFIX/$JONJ_LIBDIR", ICONV_SHARED_LIBADD)
        ;;

      bsd [)]
        JONJ_DEFINE([HAVE_BSD_ICONV],1,[ext/iconv])
        AC_DEFINE([HAVE_BSD_ICONV],1,[Konstantin Chuguev's iconv implementation])
        JONJ_DEFINE([JONJ_ICONV_IMPL],[\"BSD iconv\"],[ext/iconv])
        AC_DEFINE([JONJ_ICONV_IMPL],["BSD iconv"],[Which iconv implementation to use])
        ;;

      glibc [)]
        JONJ_DEFINE([HAVE_GLIBC_ICONV],1,[ext/iconv])
        AC_DEFINE([HAVE_GLIBC_ICONV],1,[glibc's iconv implementation])
        JONJ_DEFINE([JONJ_ICONV_IMPL],[\"glibc\"],[ext/iconv])
        AC_DEFINE([JONJ_ICONV_IMPL],["glibc"],[Which iconv implementation to use])
        ;;
      ibm [)]
        JONJ_DEFINE([HAVE_IBM_ICONV],1,[ext/iconv])
        AC_DEFINE([HAVE_IBM_ICONV],1,[IBM iconv implementation])
        JONJ_DEFINE([JONJ_ICONV_IMPL],[\"IBM iconv\"],[ext/iconv])
        AC_DEFINE([JONJ_ICONV_IMPL],["IBM iconv"],[Which iconv implementation to use])
        ;;
    esac

    AC_MSG_CHECKING([if iconv supports errno])
    AC_TRY_RUN([
#include <$JONJ_ICONV_H_PATH>
#include <errno.h>

int main() {
  iconv_t cd;
  cd = iconv_open( "*blahblah*", "*blahblah*" );
  if (cd == (iconv_t)(-1)) {
    if (errno == EINVAL) {
      return 0;
  } else {
      return 1;
    }
  }
  iconv_close( cd );
  return 2;
}
    ],[
      AC_MSG_RESULT(yes)
      JONJ_DEFINE([ICONV_SUPPORTS_ERRNO],1,[ext/iconv])
      AC_DEFINE([ICONV_SUPPORTS_ERRNO],1,[Whether iconv supports error no or not])
    ],[
      AC_MSG_RESULT(no)
      JONJ_DEFINE([ICONV_SUPPORTS_ERRNO],0,[ext/iconv])
      AC_DEFINE([ICONV_SUPPORTS_ERRNO],0,[Whether iconv supports error no or not])
    ],[
      AC_MSG_RESULT(no, cross-compiling)
      JONJ_DEFINE([ICONV_SUPPORTS_ERRNO],0,[ext/iconv])
      AC_DEFINE([ICONV_SUPPORTS_ERRNO],0,[Whether iconv supports error no or not])
    ])

    AC_MSG_CHECKING([if iconv supports //IGNORE])
    AC_TRY_RUN([
#include <$JONJ_ICONV_H_PATH>
#include <stdlib.h>

int main() {
  iconv_t cd = iconv_open( "UTF-8//IGNORE", "UTF-8" );
  char *in_p = "\xC3\xC3\xC3\xB8";
  size_t in_left = 4, out_left = 4096;
  char *out = malloc(out_left);
  char *out_p = out;
  size_t result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
  if(result == (size_t)-1) {
    return 1;
  }
  return 0;
}
   ],[
      AC_MSG_RESULT(yes)
      JONJ_DEFINE([ICONV_BROKEN_IGNORE],0,[ext/iconv])
      AC_DEFINE([ICONV_BROKEN_IGNORE],0,[Whether iconv supports IGNORE])
    ],[
      AC_MSG_RESULT(no)
      JONJ_DEFINE([ICONV_BROKEN_IGNORE],1,[ext/iconv])
      AC_DEFINE([ICONV_BROKEN_IGNORE],1,[Whether iconv supports IGNORE])
    ],[
      AC_MSG_RESULT(no, cross-compiling)
      JONJ_DEFINE([ICONV_BROKEN_IGNORE],0,[ext/iconv])
      AC_DEFINE([ICONV_BROKEN_IGNORE],0,[Whether iconv supports IGNORE])
    ])

    AC_MSG_CHECKING([if your cpp allows macro usage in include lines])
    AC_TRY_COMPILE([
#define FOO <$JONJ_ICONV_H_PATH>
#include FOO
    ], [], [
      AC_MSG_RESULT([yes])
      JONJ_DEFINE([JONJ_ICONV_H_PATH], [<$JONJ_ICONV_H_PATH>],[ext/iconv])
      AC_DEFINE_UNQUOTED([JONJ_ICONV_H_PATH], [<$JONJ_ICONV_H_PATH>], [Path to iconv.h])
    ], [
      AC_MSG_RESULT([no])
    ])

    JONJ_NEW_EXTENSION(iconv, iconv.c, $ext_shared,, [-I\"$JONJ_ICONV_PREFIX/include\"])
    JONJ_SUBST(ICONV_SHARED_LIBADD)
    JONJ_INSTALL_HEADERS([ext/iconv/])
  else
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
fi
