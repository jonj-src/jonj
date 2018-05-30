dnl $Id$
dnl config.m4 for extension pdo_mysql
dnl vim: se ts=2 sw=2 et:

JONJ_ARG_WITH(pdo-mysql, for MySQL support for PDO,
[  --with-pdo-mysql[=DIR]    PDO: MySQL support. DIR is the MySQL base directory
                          If no value or mysqlnd is passed as DIR, the
                          MySQL native driver will be used])

if test -z "$JONJ_ZLIB_DIR"; then
  JONJ_ARG_WITH(zlib-dir, for the location of libz,
  [  --with-zlib-dir[=DIR]     PDO_MySQL: Set the path to libz install prefix], no, no)
fi

if test "$JONJ_PDO_MYSQL" != "no"; then

  if test "$JONJ_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  AC_DEFUN([PDO_MYSQL_LIB_CHK], [
    str="$PDO_MYSQL_DIR/$1/libmysqlclient*"
    for j in `echo $str`; do
      if test -r $j; then
        PDO_MYSQL_LIB_DIR=$PDO_MYSQL_DIR/$1
        break 2
      fi
    done
  ])

  if test "$JONJ_PDO_MYSQL" != "yes" && test "$JONJ_PDO_MYSQL" != "mysqlnd"; then
    if test -f $JONJ_PDO_MYSQL && test -x $JONJ_PDO_MYSQL ; then
      PDO_MYSQL_CONFIG=$JONJ_PDO_MYSQL
    else
      if test -d "$JONJ_PDO_MYSQL" ; then
        if test -x "$JONJ_PDO_MYSQL/bin/mysql_config" ; then
          PDO_MYSQL_CONFIG="$JONJ_PDO_MYSQL/bin/mysql_config"
        else
          PDO_MYSQL_DIR="$JONJ_PDO_MYSQL"
        fi
      fi
    fi
  fi
  
  if test "$JONJ_PDO_MYSQL" = "yes" || test "$JONJ_PDO_MYSQL" = "mysqlnd"; then
    dnl enables build of mysqnd library
    JONJ_MYSQLND_ENABLED=yes
    AC_DEFINE([PDO_USE_MYSQLND], 1, [Whether pdo_mysql uses mysqlnd])
  else
    AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])

    AC_MSG_CHECKING([for mysql_config])
    if test -n "$PDO_MYSQL_CONFIG"; then
      AC_MSG_RESULT($PDO_MYSQL_CONFIG)
      if test "x$SED" = "x"; then
        AC_PATH_PROG(SED, sed)
      fi
      if test "$enable_maintainer_zts" = "yes"; then
        PDO_MYSQL_LIBNAME=mysqlclient_r
        PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs_r | $SED -e "s/'//g"`
      else
        PDO_MYSQL_LIBNAME=mysqlclient
        PDO_MYSQL_LIBS=`$PDO_MYSQL_CONFIG --libs | $SED -e "s/'//g"`
      fi
      PDO_MYSQL_INCLUDE=`$PDO_MYSQL_CONFIG --cflags | $SED -e "s/'//g"`
    elif test -n "$PDO_MYSQL_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_CHECKING([for mysql install under $PDO_MYSQL_DIR])
      if test -r $PDO_MYSQL_DIR/include/mysql; then
        PDO_MYSQL_INC_DIR=$PDO_MYSQL_DIR/include/mysql
      else
        PDO_MYSQL_INC_DIR=$PDO_MYSQL_DIR/include
      fi
      if test -r $PDO_MYSQL_DIR/$JONJ_LIBDIR/mysql; then
        PDO_MYSQL_LIB_DIR=$PDO_MYSQL_DIR/$JONJ_LIBDIR/mysql
      else
        PDO_MYSQL_LIB_DIR=$PDO_MYSQL_DIR/$JONJ_LIBDIR
      fi

      if test -r "$PDO_MYSQL_LIB_DIR"; then
        AC_MSG_RESULT([libs under $PDO_MYSQL_LIB_DIR; seems promising])
      else
        AC_MSG_RESULT([can not find it])
        AC_MSG_ERROR([Unable to find your mysql installation])
      fi

      JONJ_ADD_INCLUDE($PDO_MYSQL_INC_DIR)
      PDO_MYSQL_INCLUDE=-I$PDO_MYSQL_INC_DIR
    else
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Unable to find your mysql installation])
    fi

    JONJ_CHECK_LIBRARY($PDO_MYSQL_LIBNAME, mysql_commit,
    [
      JONJ_EVAL_INCLINE($PDO_MYSQL_INCLUDE)
      JONJ_EVAL_LIBLINE($PDO_MYSQL_LIBS, PDO_MYSQL_SHARED_LIBADD)
    ],[
      if test "$JONJ_ZLIB_DIR" != "no"; then
        JONJ_ADD_LIBRARY_WITH_PATH(z, $JONJ_ZLIB_DIR, PDO_MYSQL_SHARED_LIBADD)
        JONJ_CHECK_LIBRARY($PDO_MYSQL_LIBNAME, mysql_commit, [], [
          AC_MSG_ERROR([PDO_MYSQL configure failed, MySQL 4.1 needed. Please check config.log for more information.])
        ], [
          -L$JONJ_ZLIB_DIR/$JONJ_LIBDIR -L$PDO_MYSQL_LIB_DIR 
        ])  
        PDO_MYSQL_LIBS="$PDO_MYSQL_LIBS -L$JONJ_ZLIB_DIR/$JONJ_LIBDIR -lz"
      else
        JONJ_ADD_LIBRARY(z,, PDO_MYSQL_SHARED_LIBADD)
        JONJ_CHECK_LIBRARY($PDO_MYSQL_LIBNAME, mysql_query, [], [
          AC_MSG_ERROR([Try adding --with-zlib-dir=<DIR>. Please check config.log for more information.])
        ], [
          -L$PDO_MYSQL_LIB_DIR
        ])   
        PDO_MYSQL_LIBS="$PDO_MYSQL_LIBS -lz"
      fi

      JONJ_EVAL_INCLINE($PDO_MYSQL_INCLUDE)
      JONJ_EVAL_LIBLINE($PDO_MYSQL_LIBS, PDO_MYSQL_SHARED_LIBADD)
    ],[
      $PDO_MYSQL_LIBS
    ])
  fi

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

  if test -n "$PDO_MYSQL_CONFIG"; then
    PDO_MYSQL_SOCKET=`$PDO_MYSQL_CONFIG --socket`
    AC_DEFINE_UNQUOTED(PDO_MYSQL_UNIX_ADDR, "$PDO_MYSQL_SOCKET", [ ])
  fi

  dnl fix after renaming to pdo_mysql
  JONJ_NEW_EXTENSION(pdo_mysql, pdo_mysql.c mysql_driver.c mysql_statement.c, $ext_shared,,-I$pdo_cv_inc_path -I)
  ifdef([JONJ_ADD_EXTENSION_DEP],
  [
    JONJ_ADD_EXTENSION_DEP(pdo_mysql, pdo)
    if test "$JONJ_MYSQL" = "mysqlnd"; then
      JONJ_ADD_EXTENSION_DEP(pdo_mysql, mysqlnd)
    fi
  ])
  PDO_MYSQL_MODULE_TYPE=external

  JONJ_SUBST(PDO_MYSQL_SHARED_LIBADD)
  JONJ_SUBST_OLD(PDO_MYSQL_MODULE_TYPE)
fi
