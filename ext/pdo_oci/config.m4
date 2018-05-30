dnl $Id$
dnl config.m4 for extension pdo_oci
dnl vim:et:sw=2:ts=2:

SUPPORTED_LIB_VERS="9.0 10.1 11.1 12.1"  # The lib vers are not bumped when the DB version is bumped
AC_DEFUN([AC_PDO_OCI_VERSION],[
  AC_MSG_CHECKING([Oracle version])
  for OCI_VER in $SUPPORTED_LIB_VERS; do
    if test -f $PDO_OCI_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.$OCI_VER; then
      PDO_OCI_VERSION="$OCI_VER"
    fi
  done
  if test -z "$PDO_OCI_VERSION"; then
    AC_MSG_ERROR([Oracle required OCI8 libraries not found under $PDO_OCI_DIR])
  fi
  AC_MSG_RESULT($PDO_OCI_VERSION)
])                                                                                                                                                                

AC_DEFUN([AC_PDO_OCI_CHECK_LIB_DIR],[
  AC_CHECK_SIZEOF(long int, 4)
  AC_MSG_CHECKING([if we're on a 64-bit platform])
  if test "$ac_cv_sizeof_long_int" = "4" ; then
    AC_MSG_RESULT([no])
    TMP_PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib32"
  else
    AC_MSG_RESULT([yes])
    TMP_PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib"
  fi

  AC_MSG_CHECKING([OCI8 libraries dir])
  if test -d "$PDO_OCI_DIR/lib" && test ! -d "$PDO_OCI_DIR/lib32"; then
    PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib"
  elif test ! -d "$PDO_OCI_DIR/lib" && test -d "$PDO_OCI_DIR/lib32"; then
    PDO_OCI_LIB_DIR="$PDO_OCI_DIR/lib32"
  elif test -d "$PDO_OCI_DIR/lib" && test -d "$PDO_OCI_DIR/lib32"; then
    PDO_OCI_LIB_DIR=$TMP_PDO_OCI_LIB_DIR
  else
    AC_MSG_ERROR([Oracle required OCI8 libraries not found])
  fi
  AC_MSG_RESULT($PDO_OCI_LIB_DIR)
])

JONJ_ARG_WITH(pdo-oci, Oracle OCI support for PDO,
[  --with-pdo-oci[=DIR]      PDO: Oracle OCI support. DIR defaults to \$ORACLE_HOME.
                          Use --with-pdo-oci=instantclient,prefix,version 
                          for an Oracle Instant Client SDK. 
                          For example on Linux with 11.2 RPMs use:
                            --with-pdo-oci=instantclient,/usr,11.2
                          With 10.2 RPMs use:
                            --with-pdo-oci=instantclient,/usr,10.2.0.4])

if test "$JONJ_PDO_OCI" != "no"; then

  if test "$JONJ_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  AC_MSG_CHECKING([Oracle Install-Dir])
  if test "$JONJ_PDO_OCI" = "yes" || test -z "$JONJ_PDO_OCI"; then
    PDO_OCI_DIR=$ORACLE_HOME
  else
    PDO_OCI_DIR=$JONJ_PDO_OCI
  fi
  AC_MSG_RESULT($JONJ_PDO_OCI)

  AC_MSG_CHECKING([if that is sane])
  if test -z "$PDO_OCI_DIR"; then
    AC_MSG_ERROR([
You need to tell me where to find your Oracle Instant Client SDK, or set ORACLE_HOME.
])
  else
    AC_MSG_RESULT([yes])
  fi

  if test "instantclient" = "`echo $PDO_OCI_DIR | cut -d, -f1`" ; then
    AC_CHECK_SIZEOF(long int, 4)
    if test "$ac_cv_sizeof_long_int" = "4" ; then
      PDO_OCI_CLIENT_DIR="client"
    else
      PDO_OCI_CLIENT_DIR="client64"
    fi
    PDO_OCI_IC_PREFIX="`echo $PDO_OCI_DIR | cut -d, -f2`"
    PDO_OCI_IC_VERS="`echo $PDO_OCI_DIR | cut -d, -f3`"
    if test -n "$PDO_OCI_IC_VERS"; then
      PDO_OCI_IC_MAJ_VER="`echo $PDO_OCI_IC_VERS | cut -d. -f1`"
      if test "$PDO_OCI_IC_MAJ_VER" -ge 11; then
        # From 11.1.0.7 the RPM path only has an X.Y component
        PDO_OCI_IC_VERS="`echo $PDO_OCI_IC_VERS | cut -d. -f1-2`"
      fi
    fi
    AC_MSG_CHECKING([for oci.h])
    if test -f $PDO_OCI_IC_PREFIX/include/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR/oci.h ; then
      JONJ_ADD_INCLUDE($PDO_OCI_IC_PREFIX/include/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR)
      AC_MSG_RESULT($PDO_OCI_IC_PREFIX/include/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR)
    elif test -f $PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR/include/oci.h ; then
      JONJ_ADD_INCLUDE($PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR/include)
      AC_MSG_RESULT($PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR/include)
    elif test -f $PDO_OCI_IC_PREFIX/sdk/include/oci.h ; then
      JONJ_ADD_INCLUDE($PDO_OCI_IC_PREFIX/sdk/include)
      AC_MSG_RESULT($PDO_OCI_IC_PREFIX/sdk/include)
    elif test -f $PDO_OCI_IC_PREFIX/$PDO_OCI_CLIENT_DIR/include/oci.h ; then
      JONJ_ADD_INCLUDE($PDO_OCI_IC_PREFIX/$PDO_OCI_CLIENT_DIR/include)
      AC_MSG_RESULT($PDO_OCI_IC_PREFIX/$PDO_OCI_CLIENT_DIR/include)
    else
      AC_MSG_ERROR([I'm too dumb to figure out where the include dir is in your Instant Client install])
    fi
    if test -f "$PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME" ; then
    PDO_OCI_LIB_DIR="$PDO_OCI_IC_PREFIX/lib/oracle/$PDO_OCI_IC_VERS/$PDO_OCI_CLIENT_DIR/lib"
    elif test -f "$PDO_OCI_IC_PREFIX/$PDO_OCI_CLIENT_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME" ; then
      PDO_OCI_LIB_DIR="$PDO_OCI_IC_PREFIX/$PDO_OCI_CLIENT_DIR/lib"
    elif test -f "$PDO_OCI_IC_PREFIX/libclntsh.$SHLIB_SUFFIX_NAME" ; then
      PDO_OCI_LIB_DIR="$PDO_OCI_IC_PREFIX"
    else
      AC_MSG_ERROR([I'm too dumb to figure out where the libraries are in your Instant Client install])
    fi
    PDO_OCI_VERSION="`echo $PDO_OCI_IC_VERS | cut -d. -f1-2`"
  else
    AC_PDO_OCI_CHECK_LIB_DIR($PDO_OCI_DIR)

    if test -d "$PDO_OCI_DIR/rdbms/public"; then
      JONJ_ADD_INCLUDE($PDO_OCI_DIR/rdbms/public)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/rdbms/public"
    fi
    if test -d "$PDO_OCI_DIR/rdbms/demo"; then
      JONJ_ADD_INCLUDE($PDO_OCI_DIR/rdbms/demo)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/rdbms/demo"
    fi
    if test -d "$PDO_OCI_DIR/network/public"; then
      JONJ_ADD_INCLUDE($PDO_OCI_DIR/network/public)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/network/public"
    fi
    if test -d "$PDO_OCI_DIR/plsql/public"; then
      JONJ_ADD_INCLUDE($PDO_OCI_DIR/plsql/public)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/plsql/public"
    fi
    if test -d "$PDO_OCI_DIR/include"; then
      JONJ_ADD_INCLUDE($PDO_OCI_DIR/include)
      PDO_OCI_INCLUDES="$PDO_OCI_INCLUDES -I$PDO_OCI_DIR/include"
    fi

    if test -f "$PDO_OCI_LIB_DIR/sysliblist"; then
      JONJ_EVAL_LIBLINE(`cat $PDO_OCI_LIB_DIR/sysliblist`, PDO_OCI_SYSLIB)
    elif test -f "$PDO_OCI_DIR/rdbms/lib/sysliblist"; then
      JONJ_EVAL_LIBLINE(`cat $PDO_OCI_DIR/rdbms/lib/sysliblist`, PDO_OCI_SYSLIB)
    fi
    AC_PDO_OCI_VERSION($PDO_OCI_DIR)
  fi

  case $PDO_OCI_VERSION in
    9.0|10.1|10.2|11.1|11.2|12.1)
      JONJ_ADD_LIBRARY(clntsh, 1, PDO_OCI_SHARED_LIBADD)
      ;;

    *)
      AC_MSG_ERROR(Unsupported Oracle version $PDO_OCI_VERSION)
      ;;
  esac

  JONJ_ADD_LIBPATH($PDO_OCI_LIB_DIR, PDO_OCI_SHARED_LIBADD)

  JONJ_CHECK_LIBRARY(clntsh, OCIEnvCreate,
  [
    AC_DEFINE(HAVE_OCIENVCREATE,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  JONJ_CHECK_LIBRARY(clntsh, OCIEnvNlsCreate,
  [
    AC_DEFINE(HAVE_OCIENVNLSCREATE,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  dnl
  dnl Check if we need to add -locijdbc8 
  dnl
  JONJ_CHECK_LIBRARY(clntsh, OCILobIsTemporary,
  [
    AC_DEFINE(HAVE_OCILOBISTEMPORARY,1,[ ])
  ], [
    JONJ_CHECK_LIBRARY(ocijdbc8, OCILobIsTemporary,
    [
      JONJ_ADD_LIBRARY(ocijdbc8, 1, PDO_OCI_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCILOBISTEMPORARY,1,[ ])
    ], [], [
      -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
    ])
  ], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  dnl
  dnl Check if we have collections
  dnl
  JONJ_CHECK_LIBRARY(clntsh, OCICollAssign,
  [
    AC_DEFINE(HAVE_OCICOLLASSIGN,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  dnl Scrollable cursors?
  JONJ_CHECK_LIBRARY(clntsh, OCIStmtFetch2,
  [
     AC_DEFINE(HAVE_OCISTMTFETCH2,1,[ ])
  ], [], [
    -L$PDO_OCI_LIB_DIR $PDO_OCI_SHARED_LIBADD
  ])

  ifdef([JONJ_CHECK_PDO_INCLUDES],
  [
    JONJ_CHECK_PDO_INCLUDES
  ],[
    AC_MSG_CHECKING([for PDO includes])
    if test -f $abs_srcdir/include/jonj/ext/pdo/jonj_pdo_driver.h; then
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $abs_srcdir/ext/pdo/jonj_pdo_driver.h; then
      pdo_cv_inc_path=$abs_srcdir/ext
    elif test -f $prefix/include/jonj/ext/pdo/jonj_pdo_driver.h; then
      pdo_cv_inc_path=$prefix/include/jonj/ext
    else
      AC_MSG_ERROR([Cannot find jonj_pdo_driver.h.])
    fi
    AC_MSG_RESULT($pdo_cv_inc_path)
  ])

  JONJ_NEW_EXTENSION(pdo_oci, pdo_oci.c oci_driver.c oci_statement.c, $ext_shared,,-I$pdo_cv_inc_path)

  JONJ_SUBST_OLD(PDO_OCI_SHARED_LIBADD)
  JONJ_SUBST_OLD(PDO_OCI_DIR)
  JONJ_SUBST_OLD(PDO_OCI_VERSION)

  ifdef([JONJ_ADD_EXTENSION_DEP],
  [
    JONJ_ADD_EXTENSION_DEP(pdo_oci, pdo)
  ])

  AC_DEFINE_UNQUOTED(JONJ_PDO_OCI_CLIENT_VERSION, "$PDO_OCI_VERSION", [ ])
fi

