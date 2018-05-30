dnl ## $Id$ -*- sh -*-

JONJ_ARG_WITH(continuity, for Continuity support,
[  --with-continuity=DIR   Build JONJ as Continuity Server module. 
                          DIR is path to the installed Continuity Server root], no, no)

if test "$JONJ_CONTINUITY" != "no"; then
  if test ! -d $JONJ_CONTINUITY; then
    AC_MSG_ERROR([Please specify the path to the root of your Continuity server using --with-continuity=DIR])
  fi
  AC_MSG_CHECKING([for Continuity include files])
  if test -d $JONJ_CONTINUITY/include ; then
    CAPI_INCLUDE=$JONJ_CONTINUITY/include
    AC_MSG_RESULT([Continuity Binary Distribution])
  else
    AC_MSG_ERROR([Cannot find your CAPI include files in either DIR/src or DIR/include])
  fi

  JONJ_SELECT_SAPI(continuity, shared, capi.c)
  JONJ_ADD_INCLUDE($CAPI_INCLUDE)
  JONJ_BUILD_THREAD_SAFE
  AC_DEFINE(HAVE_CONTINUITY, 1, [Whether you have a Continuity Server])
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$JONJ_CONTINUITY/lib/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
