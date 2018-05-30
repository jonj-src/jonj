dnl
dnl $Id$
dnl

JONJ_ARG_WITH(isapi, for Zeus ISAPI support,
[  --with-isapi[=DIR]        Build JONJ as an ISAPI module for use with Zeus], no, no)

if test "$JONJ_ISAPI" != "no"; then
  if test "$JONJ_ISAPI" = "yes"; then
    ZEUSPATH=/usr/local/zeus # the default
  else
    ZEUSPATH=$JONJ_ISAPI
  fi
  test -f "$ZEUSPATH/web/include/httpext.h" || AC_MSG_ERROR(Unable to find httpext.h in $ZEUSPATH/web/include)
  JONJ_BUILD_THREAD_SAFE
  AC_DEFINE(WITH_ZEUS, 1, [ ])
  JONJ_ADD_INCLUDE($ZEUSPATH/web/include)
  JONJ_SELECT_SAPI(isapi, shared, jonj5isapi.c)
  INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$ZEUSPATH/web/bin/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
