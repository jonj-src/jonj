dnl
dnl $Id$
dnl

AC_DEFUN([MYSQL_LIB_CHK], [
  str="$MYSQL_DIR/$1/lib$MYSQL_LIBNAME.*"
  for j in `echo $str`; do
    if test -r $j; then
      MYSQL_LIB_DIR=$MYSQL_DIR/$1
      break 2
    fi
  done
])

AC_DEFUN([JONJ_MYSQL_SOCKET_SEARCH], [
  for i in  \
    /var/run/mysqld/mysqld.sock \
    /var/tmp/mysql.sock \
    /var/run/mysql/mysql.sock \
    /var/lib/mysql/mysql.sock \
    /var/mysql/mysql.sock \
    /usr/local/mysql/var/mysql.sock \
    /Private/tmp/mysql.sock \
    /private/tmp/mysql.sock \
    /tmp/mysql.sock \
  ; do
    if test -r $i; then
      MYSQL_SOCK=$i
      break 2
    fi
  done

  if test -n "$MYSQL_SOCK"; then
    AC_DEFINE_UNQUOTED(JONJ_MYSQL_UNIX_SOCK_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  else
    AC_MSG_RESULT([no])
  fi
])


JONJ_ARG_WITH(mysql, for MySQL support,
[  --with-mysql[=DIR]        Include MySQL support.  DIR is the MySQL base
                          directory, if no DIR is passed or the value is
                          mysqlnd the MySQL native driver will be used])

JONJ_ARG_WITH(mysql-sock, for specified location of the MySQL UNIX socket,
[  --with-mysql-sock[=SOCKPATH]
                          MySQL/MySQLi/PDO_MYSQL: Location of the MySQL unix socket pointer.
                          If unspecified, the default locations are searched], no, no)

if test -z "$JONJ_ZLIB_DIR"; then
  JONJ_ARG_WITH(zlib-dir, for the location of libz, 
  [  --with-zlib-dir[=DIR]     MySQL: Set the path to libz install prefix], no, no)
fi

if test "$JONJ_MYSQL" = "yes" || test "$JONJ_MYSQL" = "mysqlnd"; then
  dnl enables build of mysqnd library
  JONJ_MYSQLND_ENABLED=yes

elif test "$JONJ_MYSQL" != "no"; then
  MYSQL_DIR=
  MYSQL_INC_DIR=

  if test -r $JONJ_MYSQL/include/mysql/mysql.h; then
    MYSQL_DIR=$JONJ_MYSQL
    MYSQL_INC_DIR=$JONJ_MYSQL/include/mysql
    break
  elif test -r $JONJ_MYSQL/include/mysql.h; then
    MYSQL_DIR=$JONJ_MYSQL
    MYSQL_INC_DIR=$JONJ_MYSQL/include
    break
  fi

  if test -z "$MYSQL_DIR"; then
    AC_MSG_ERROR([Cannot find MySQL header files under $JONJ_MYSQL.
Note that the MySQL client library is not bundled anymore!])
  fi

  if test "$enable_maintainer_zts" = "yes"; then
    MYSQL_LIBNAME=mysqlclient_r
  else
    MYSQL_LIBNAME=mysqlclient
  fi
  case $host_alias in
    *netware*[)]
      MYSQL_LIBNAME=mysql
      ;;
  esac

  dnl for compat with JONJ 4 build system
  if test -z "$JONJ_LIBDIR"; then
    JONJ_LIBDIR=lib
  fi

  for i in $JONJ_LIBDIR $JONJ_LIBDIR/mysql; do
    MYSQL_LIB_CHK($i)
  done

  if test -z "$MYSQL_LIB_DIR"; then
    MYSQL_LIB_CHK(lib/x86_64-linux-gnu)
  fi
  if test -z "$MYSQL_LIB_DIR"; then
    MYSQL_LIB_CHK(lib/i386-linux-gnu)
  fi

  if test -z "$MYSQL_LIB_DIR"; then
    AC_MSG_ERROR([Cannot find lib$MYSQL_LIBNAME under $MYSQL_DIR.
Note that the MySQL client library is not bundled anymore!])
  fi

  JONJ_CHECK_LIBRARY($MYSQL_LIBNAME, mysql_close, [ ],
  [
    if test "$JONJ_ZLIB_DIR" != "no"; then
      JONJ_ADD_LIBRARY_WITH_PATH(z, $JONJ_ZLIB_DIR, MYSQL_SHARED_LIBADD)
      JONJ_CHECK_LIBRARY($MYSQL_LIBNAME, mysql_error, [], [
        AC_MSG_ERROR([mysql configure failed. Please check config.log for more information.])
      ], [
        -L$JONJ_ZLIB_DIR/$JONJ_LIBDIR -L$MYSQL_LIB_DIR 
      ])  
      MYSQL_LIBS="-L$JONJ_ZLIB_DIR/$JONJ_LIBDIR -lz"
    else
      JONJ_ADD_LIBRARY(z,, MYSQL_SHARED_LIBADD)
      JONJ_CHECK_LIBRARY($MYSQL_LIBNAME, mysql_errno, [], [
        AC_MSG_ERROR([Try adding --with-zlib-dir=<DIR>. Please check config.log for more information.])
      ], [
        -L$MYSQL_LIB_DIR
      ])   
      MYSQL_LIBS="-lz"
    fi
  ], [
    -L$MYSQL_LIB_DIR 
  ])

  JONJ_ADD_LIBRARY_WITH_PATH($MYSQL_LIBNAME, $MYSQL_LIB_DIR, MYSQL_SHARED_LIBADD)
  JONJ_ADD_INCLUDE($MYSQL_INC_DIR)

  MYSQL_MODULE_TYPE=external
  MYSQL_LIBS="-L$MYSQL_LIB_DIR -l$MYSQL_LIBNAME $MYSQL_LIBS"
  MYSQL_INCLUDE=-I$MYSQL_INC_DIR
 
  JONJ_SUBST_OLD(MYSQL_MODULE_TYPE)
  JONJ_SUBST_OLD(MYSQL_LIBS)
  JONJ_SUBST_OLD(MYSQL_INCLUDE)
fi

dnl Enable extension
if test "$JONJ_MYSQL" != "no"; then
  AC_MSG_CHECKING([for MySQL UNIX socket location])
  if test "$JONJ_MYSQL_SOCK" != "no" && test "$JONJ_MYSQL_SOCK" != "yes"; then
    MYSQL_SOCK=$JONJ_MYSQL_SOCK
    AC_DEFINE_UNQUOTED(JONJ_MYSQL_UNIX_SOCK_ADDR, "$MYSQL_SOCK", [ ])
    AC_MSG_RESULT([$MYSQL_SOCK])
  elif test "$JONJ_MYSQL" = "yes" || test "$JONJ_MYSQL_SOCK" = "yes"; then
    JONJ_MYSQL_SOCKET_SEARCH
  else
    AC_MSG_RESULT([no])
  fi

  AC_DEFINE(HAVE_MYSQL, 1, [Whether you have MySQL])
  JONJ_NEW_EXTENSION(mysql, jonj_mysql.c, $ext_shared)
  JONJ_SUBST(MYSQL_SHARED_LIBADD)

  if test "$JONJ_MYSQL" = "yes" || test "$JONJ_MYSQL" = "mysqlnd"; then
    JONJ_ADD_EXTENSION_DEP(mysql, mysqlnd)
    AC_DEFINE([MYSQL_USE_MYSQLND], 1, [Whether mysqlnd is enabled])
  fi
fi
