dnl
dnl $Id$
dnl

JONJ_ARG_WITH(thttpd,,
[  --with-thttpd=SRCDIR    Build JONJ as thttpd module], no, no)

AC_MSG_CHECKING([for thttpd])

if test "$JONJ_THTTPD" != "no"; then
  if test ! -d $JONJ_THTTPD; then
    AC_MSG_RESULT(thttpd directory does not exist ($JONJ_THTTPD))
  fi

  JONJ_EXPAND_PATH($JONJ_THTTPD, THTTPD)
  
  if grep thttpd.2.21b $JONJ_THTTPD/version.h >/dev/null; then
    patch="test -f $THTTPD/jonj_patched || \
    (cd $THTTPD && patch -p1 < $abs_srcdir/sapi/thttpd/thttpd_patch && touch jonj_patched)"

  elif grep Premium $JONJ_THTTPD/version.h >/dev/null; then
    patch=
  else
    AC_MSG_ERROR([This version only supports thttpd-2.21b and Premium thttpd])
  fi
  JONJ_TARGET_RDYNAMIC
  INSTALL_IT="\
    echo 'JONJ_LIBS = -L. -ljonj5 \$(JONJ_LIBS) \$(EXTRA_LIBS)' > $THTTPD/jonj_makefile; \
    echo 'JONJ_LDFLAGS = \$(NATIVE_RPATHS) \$(JONJ_LDFLAGS)' >> $THTTPD/jonj_makefile; \
    echo 'JONJ_CFLAGS = \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(CPPFLAGS) \$(EXTRA_CFLAGS)' >> $THTTPD/jonj_makefile; \
    rm -f $THTTPD/jonj_thttpd.c $THTTPD/jonj_thttpd.h $THTTPD/libjonj5.a; \
    \$(LN_S) $abs_srcdir/sapi/thttpd/thttpd.c $THTTPD/jonj_thttpd.c; \
    \$(LN_S) $abs_srcdir/sapi/thttpd/jonj_thttpd.h $abs_builddir/$SAPI_STATIC $THTTPD/;\
    $patch"
  JONJ_THTTPD="yes, using $THTTPD"
  JONJ_ADD_INCLUDE($THTTPD)
  JONJ_SELECT_SAPI(thttpd, static)
fi
AC_MSG_RESULT($JONJ_THTTPD)
