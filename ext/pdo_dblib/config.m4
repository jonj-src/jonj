dnl
dnl $Id$
dnl

JONJ_ARG_WITH(pdo-dblib, for PDO_DBLIB support via FreeTDS,
[  --with-pdo-dblib[=DIR]    PDO: DBLIB-DB support.  DIR is the FreeTDS home directory])

if test "$JONJ_PDO_DBLIB" != "no"; then

  if test "$JONJ_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  if test "$JONJ_PDO_DBLIB" = "yes"; then

    for i in /usr/local /usr; do
      if test -f $i/include/sybdb.h; then
        PDO_FREETDS_INSTALLATION_DIR=$i
        PDO_FREETDS_INCLUDE_DIR=$i/include
        break
      elif test -f $i/include/freetds/sybdb.h; then
        PDO_FREETDS_INSTALLATION_DIR=$i
        PDO_FREETDS_INCLUDE_DIR=$i/include/freetds
        break;
      fi
    done

    if test -z "$PDO_FREETDS_INSTALLATION_DIR"; then
      AC_MSG_ERROR(Cannot find FreeTDS in known installation directories)
    fi

  elif test "$JONJ_PDO_DBLIB" != "no"; then

    if test -f $JONJ_PDO_DBLIB/include/sybdb.h; then
      PDO_FREETDS_INSTALLATION_DIR=$JONJ_PDO_DBLIB
      PDO_FREETDS_INCLUDE_DIR=$JONJ_PDO_DBLIB/include
    elif test -f $JONJ_PDO_DBLIB/include/freetds/sybdb.h; then
      PDO_FREETDS_INSTALLATION_DIR=$JONJ_PDO_DBLIB
      PDO_FREETDS_INCLUDE_DIR=$JONJ_PDO_DBLIB/include/freetds
    else
      AC_MSG_ERROR(Directory $JONJ_PDO_DBLIB is not a FreeTDS installation directory)
    fi
  fi  

  if test "x$JONJ_LIBDIR" = "x" ; then
    JONJ_LIBDIR=lib
  fi

  if test ! -r "$PDO_FREETDS_INSTALLATION_DIR/$JONJ_LIBDIR/libsybdb.a" && test ! -r "$PDO_FREETDS_INSTALLATION_DIR/$JONJ_LIBDIR/libsybdb.so"; then
     AC_MSG_ERROR(Could not find $PDO_FREETDS_INSTALLATION_DIR/$JONJ_LIBDIR/libsybdb.[a|so])
  fi

  JONJ_ADD_INCLUDE($PDO_FREETDS_INCLUDE_DIR)
  JONJ_ADD_LIBRARY_WITH_PATH(sybdb, $PDO_FREETDS_INSTALLATION_DIR/$JONJ_LIBDIR, PDO_DBLIB_SHARED_LIBADD)

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

  PDO_DBLIB_DEFS="-DPDO_DBLIB_FLAVOUR=\\\"freetds\\\""
  JONJ_NEW_EXTENSION(pdo_dblib, pdo_dblib.c dblib_driver.c dblib_stmt.c, $ext_shared,,-I$pdo_cv_inc_path $PDO_DBLIB_DEFS)
  AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ JONJ_ADD_LIBRARY_WITH_PATH(dnet_stub,,PDO_DBLIB_SHARED_LIBADD)
        AC_DEFINE(HAVE_LIBDNET_STUB,1,[ ])
     ])
  AC_DEFINE(HAVE_PDO_DBLIB,1,[ ])
  AC_DEFINE(HAVE_FREETDS,1,[ ])
  JONJ_SUBST(PDO_DBLIB_SHARED_LIBADD)

  ifdef([JONJ_ADD_EXTENSION_DEP],
  [
    JONJ_ADD_EXTENSION_DEP(pdo_dblib, pdo)
  ])
fi
