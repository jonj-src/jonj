dnl $Id$
dnl config.m4 for extension pdo_sqlite
dnl vim:et:sw=2:ts=2:

JONJ_ARG_WITH(pdo-sqlite, for sqlite 3 support for PDO,
[  --without-pdo-sqlite[=DIR]
                          PDO: sqlite 3 support.  DIR is the sqlite base
                          install directory [BUNDLED]], $JONJ_PDO)

if test "$JONJ_PDO_SQLITE" != "no"; then

  if test "$JONJ_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
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

  jonj_pdo_sqlite_sources_core="pdo_sqlite.c sqlite_driver.c sqlite_statement.c"

  if test "$JONJ_PDO_SQLITE" != "yes"; then
    SEARCH_PATH="$JONJ_PDO_SQLITE /usr/local /usr"     # you might want to change this
    SEARCH_FOR="/include/sqlite3.h"  # you most likely want to change this
    if test -r $JONJ_PDO_SQLITE/$SEARCH_FOR; then # path given as parameter
      PDO_SQLITE_DIR=$JONJ_PDO_SQLITE
    else # search default path list
      AC_MSG_CHECKING([for sqlite3 files in default path])
      for i in $SEARCH_PATH ; do
        if test -r $i/$SEARCH_FOR; then
          PDO_SQLITE_DIR=$i
          AC_MSG_RESULT(found in $i)
        fi
      done
    fi
    if test -z "$PDO_SQLITE_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the sqlite3 distribution])
    fi

    JONJ_ADD_INCLUDE($PDO_SQLITE_DIR/include)

    LIBNAME=sqlite3
    LIBSYMBOL=sqlite3_open

    JONJ_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
    [
      JONJ_ADD_LIBRARY_WITH_PATH($LIBNAME, $PDO_SQLITE_DIR/$JONJ_LIBDIR, PDO_SQLITE_SHARED_LIBADD)
      AC_DEFINE(HAVE_PDO_SQLITELIB,1,[ ])
    ],[
      AC_MSG_ERROR([wrong sqlite lib version or lib not found])
    ],[
      -L$PDO_SQLITE_DIR/$JONJ_LIBDIR -lm
    ])
    JONJ_CHECK_LIBRARY(sqlite3,sqlite3_key,[
      AC_DEFINE(HAVE_SQLITE3_KEY,1, [have commercial sqlite3 with crypto support])
    ])

    JONJ_SUBST(PDO_SQLITE_SHARED_LIBADD)
    JONJ_NEW_EXTENSION(pdo_sqlite, $jonj_pdo_sqlite_sources_core, $ext_shared,,-I$pdo_cv_inc_path)
  else
      # use bundled libs
      if test "$enable_maintainer_zts" = "yes"; then
        threadsafe_flags="-DSQLITE_THREADSAFE=1"
      else
        threadsafe_flags="-DSQLITE_THREADSAFE=0"
      fi

      other_flags="-DSQLITE_ENABLE_FTS3=1 -DSQLITE_CORE=1 -DSQLITE_ENABLE_COLUMN_METADATA=1"

	  dnl As long as intl is not shared we can have ICU support
      if test "$JONJ_INTL" = "yes" && test "$JONJ_INTL_SHARED" != "yes"; then
        other_flags="$other_flags -DSQLITE_ENABLE_ICU=1"
      fi

      if test "$JONJ_SQLITE3" != "yes"; then
        JONJ_ADD_SOURCES(JONJ_EXT_DIR(sqlite3), libsqlite/sqlite3.c)
      fi

      JONJ_NEW_EXTENSION(pdo_sqlite,
        $jonj_pdo_sqlite_sources_core,
        $ext_shared,,-DPDO_SQLITE_BUNDLED=1 $other_flags $threadsafe_flags -I$pdo_cv_inc_path)

      JONJ_SUBST(PDO_SQLITE_SHARED_LIBADD)
      JONJ_ADD_EXTENSION_DEP(pdo_sqlite, sqlite3)
      JONJ_ADD_INCLUDE($abs_srcdir/ext/sqlite3/libsqlite)

      AC_CHECK_FUNCS(usleep nanosleep)
      AC_CHECK_HEADERS(time.h)
  fi

  dnl Solaris fix
  JONJ_CHECK_LIBRARY(rt, fdatasync, [JONJ_ADD_LIBRARY(rt,, PDO_SQLITE_SHARED_LIBADD)])

  ifdef([JONJ_ADD_EXTENSION_DEP],
  [
    JONJ_ADD_EXTENSION_DEP(pdo_sqlite, pdo)
  ])
fi
