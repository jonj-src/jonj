dnl
dnl $Id$
dnl 

JONJ_ARG_WITH(phttpd, for PHTTPD support,
[  --with-phttpd=DIR       Build JONJ as phttpd module], no, no)

if test "$JONJ_PHTTPD" != "no"; then
  if test ! -d $JONJ_PHTTPD ; then
    AC_MSG_ERROR([You did not specify a directory])
  fi
  JONJ_BUILD_THREAD_SAFE
  JONJ_ADD_INCLUDE($JONJ_PHTTPD/include)
  AC_DEFINE(HAVE_PHTTPD, 1, [Whether you have phttpd])
  JONJ_SELECT_SAPI(phttpd, shared, phttpd.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$JONJ_PHTTPD/modules/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
