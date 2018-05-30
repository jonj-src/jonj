dnl
dnl $Id$
dnl 

AC_DEFUN([JONJ_MCRYPT_CHECK_VERSION],[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$MCRYPT_DIR/include
  AC_MSG_CHECKING(for libmcrypt version)
  AC_EGREP_CPP(yes,[
#include <mcrypt.h>
#if MCRYPT_API_VERSION >= 20021217
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 2.5.6)
  ],[
    AC_MSG_ERROR(libmcrypt version 2.5.6 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])  


JONJ_ARG_WITH(mcrypt, for mcrypt support,
[  --with-mcrypt[=DIR]       Include mcrypt support])

if test "$JONJ_MCRYPT" != "no"; then
  for i in $JONJ_MCRYPT /usr/local /usr; do
    test -f $i/include/mcrypt.h && MCRYPT_DIR=$i && break
  done

  if test -z "$MCRYPT_DIR"; then
    AC_MSG_ERROR(mcrypt.h not found. Please reinstall libmcrypt.)
  fi

  JONJ_MCRYPT_CHECK_VERSION

  JONJ_CHECK_LIBRARY(mcrypt, mcrypt_module_open, 
  [
    JONJ_ADD_LIBRARY(ltdl,, MCRYPT_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])
  ],[
    JONJ_CHECK_LIBRARY(mcrypt, mcrypt_module_open,
    [
      AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])
    ],[
      AC_MSG_ERROR([Sorry, I was not able to diagnose which libmcrypt version you have installed.])
    ],[
      -L$MCRYPT_DIR/$JONJ_LIBDIR
    ])
  ],[
    -L$MCRYPT_DIR/$JONJ_LIBDIR -lltdl
  ])

  JONJ_ADD_LIBRARY_WITH_PATH(mcrypt, $MCRYPT_DIR/$JONJ_LIBDIR, MCRYPT_SHARED_LIBADD)
  JONJ_ADD_INCLUDE($MCRYPT_DIR/include)

  JONJ_SUBST(MCRYPT_SHARED_LIBADD)
  JONJ_NEW_EXTENSION(mcrypt, mcrypt.c mcrypt_filter.c, $ext_shared)
fi
