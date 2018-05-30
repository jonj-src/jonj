dnl
dnl $Id$
dnl

JONJ_ARG_WITH(tux,,
[  --with-tux=MODULEDIR    Build JONJ as a TUX module (Linux only)], no, no)

AC_MSG_CHECKING([for TUX])
if test "$JONJ_TUX" != "no"; then
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED $JONJ_TUX/jonj5.tux.so"
  AC_CHECK_HEADERS(tuxmodule.h,[:],[AC_MSG_ERROR([Cannot find tuxmodule.h])])
  JONJ_SELECT_SAPI(tux, shared, jonj_tux.c)
  AC_MSG_RESULT([$JONJ_TUX])
else
  AC_MSG_RESULT(no)
fi
