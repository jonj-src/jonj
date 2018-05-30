dnl
dnl $Id$
dnl

JONJ_ARG_WITH(webjames,,
[  --with-webjames=SRCDIR  Build JONJ as a WebJames module (RISC OS only)], no, no)

AC_MSG_CHECKING([for webjames])

if test "$JONJ_WEBJAMES" != "no"; then
  JONJ_EXPAND_PATH($JONJ_WEBJAMES, JONJ_WEBJAMES)
  INSTALL_IT="\
    echo 'JONJ_LIBS = -l$abs_srcdir/$SAPI_STATIC \$(JONJ_LIBS) \$(EXTRA_LIBS)' > $JONJ_WEBJAMES/build/jonj; \
    echo 'JONJ_LDFLAGS = \$(NATIVE_RPATHS) \$(JONJ_LDFLAGS)' >> $JONJ_WEBJAMES/build/jonj; \
    echo 'JONJ_CFLAGS = -DJONJ \$(COMMON_FLAGS) \$(EXTRA_CFLAGS) -I$abs_srcdir/sapi/webjames' >> $JONJ_WEBJAMES/build/jonj;"
  JONJ_ADD_INCLUDE($JONJ_WEBJAMES)
  JONJ_SELECT_SAPI(webjames, static, webjames.c)
  AC_MSG_RESULT([yes, using $JONJ_WEBJAMES])
else
  AC_MSG_RESULT(no)
fi
