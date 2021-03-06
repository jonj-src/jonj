dnl
dnl $Id$
dnl

JONJ_ARG_WITH(pi3web, for Pi3Web support,
[  --with-pi3web[=DIR]       Build JONJ as Pi3Web module], no, no)

if test "$JONJ_PI3WEB" != "no"; then
  if test "$JONJ_PI3WEB" = "yes"; then
    PI3PATH=../.. # the default
  else
    PI3PATH=$JONJ_PI3WEB
  fi
  test -f "$PI3PATH/PiAPI/PiAPI.h" || AC_MSG_ERROR([Unable to find PiAPI.h in $PI3PATH/PiAPI])
  JONJ_BUILD_THREAD_SAFE
  AC_DEFINE(WITH_PI3WEB, 1, [whether you want Pi3Web support])
  JONJ_ADD_INCLUDE($PI3PATH/PiAPI)
  JONJ_ADD_INCLUDE($PI3PATH/Pi2API)
  JONJ_ADD_INCLUDE($PI3PATH/Pi3API)
  JONJ_ADD_INCLUDE($PI3PATH/JONJ5)
  JONJ_SELECT_SAPI(pi3web, shared, pi3web_sapi.c)
  INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PI3PATH/bin/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
