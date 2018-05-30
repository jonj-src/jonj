dnl
dnl $Id$
dnl

JONJ_ARG_WITH(roxen,, 
[  --with-roxen=DIR        Build JONJ as a Pike module. DIR is the base Roxen
                          directory, normally /usr/local/roxen/server], no, no)

JONJ_ARG_ENABLE(roxen-zts, whether Roxen module is build using ZTS,
[  --enable-roxen-zts      ROXEN: Build the Roxen module using Zend Thread Safety], no, no)

RESULT=
AC_MSG_CHECKING([for Roxen/Pike support])
if test "$JONJ_ROXEN" != "no"; then
  if test ! -d $JONJ_ROXEN ; then
    AC_MSG_ERROR([You did not specify a directory])
  fi
  if test -f $JONJ_ROXEN/bin/roxen; then
    PIKE=$JONJ_ROXEN/bin/roxen
  elif test -f $JONJ_ROXEN/bin/pike; then
    PIKE=$JONJ_ROXEN/bin/pike
  else
    AC_MSG_ERROR([Could not find a pike in $JONJ_ROXEN/bin/])
  fi

  if $PIKE -e 'float v; catch(v = __VERSION__ + (__BUILD__/10000.0)); if(v < 0.7079) exit(1); exit(0);'; then
    PIKE_MODULE_DIR=`$PIKE --show-paths 2>&1| grep '^Module' | sed -e 's/.*: //'`
    PIKE_INCLUDE_DIR=`echo $PIKE_MODULE_DIR | sed -e 's,lib/pike/modules,include/pike,' -e 's,lib/modules,include/pike,'`
    if test -z "$PIKE_INCLUDE_DIR" || test -z "$PIKE_MODULE_DIR"; then
      AC_MSG_ERROR([Failed to figure out Pike module and include directories])
    fi
  else
    AC_MSG_ERROR([Roxen/JONJ requires Pike 0.7.79 or newer])
  fi
   
  JONJ_ADD_INCLUDE($PIKE_INCLUDE_DIR)
  AC_DEFINE(HAVE_ROXEN, 1, [Whether you use Roxen])
  JONJ_SELECT_SAPI(roxen, shared, roxen.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED $PIKE_MODULE_DIR/JONJ5.so"
  RESULT="yes
  Pike binary used:      $PIKE
  Pike include dir:      $PIKE_INCLUDE_DIR
  Pike module directory: $PIKE_MODULE_DIR"
  PIKE_INCLUDE_DIR=" -I$PIKE_INCLUDE_DIR "

  if test "$JONJ_ROXEN_ZTS" != "no"; then
    JONJ_BUILD_THREAD_SAFE
    AC_DEFINE(ROXEN_USE_ZTS, 1, [Whether to use Roxen in ZTS mode])
  fi
fi
AC_MSG_RESULT([$RESULT])

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
