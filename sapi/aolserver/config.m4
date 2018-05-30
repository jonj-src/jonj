dnl
dnl $Id$
dnl

JONJ_ARG_WITH(aolserver,,
[  --with-aolserver=DIR    Specify path to the installed AOLserver], no, no)

AC_MSG_CHECKING([for AOLserver support])

if test "$JONJ_AOLSERVER" != "no"; then
  if test -d "$JONJ_AOLSERVER/include"; then
    JONJ_AOLSERVER_SRC=$JONJ_AOLSERVER
  fi
  if test -z "$JONJ_AOLSERVER_SRC" || test ! -d $JONJ_AOLSERVER_SRC/include; then
    AC_MSG_ERROR(Please specify the path to the source distribution of AOLserver using --with-aolserver-src=DIR)
  fi
  if test ! -d $JONJ_AOLSERVER/bin ; then
    AC_MSG_ERROR(Please specify the path to the root of AOLserver using --with-aolserver=DIR)
  fi
  JONJ_BUILD_THREAD_SAFE
  JONJ_ADD_INCLUDE($JONJ_AOLSERVER_SRC/include)
  AC_DEFINE(HAVE_AOLSERVER,1,[Whether you have AOLserver])
  JONJ_SELECT_SAPI(aolserver, shared, aolserver.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$JONJ_AOLSERVER/bin/"
fi

AC_MSG_RESULT([$JONJ_AOLSERVER])

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
