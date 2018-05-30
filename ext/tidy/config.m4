dnl
dnl $Id$
dnl

JONJ_ARG_WITH(tidy,for TIDY support,
[  --with-tidy[=DIR]         Include TIDY support])

if test "$JONJ_TIDY" != "no"; then

  if test "$JONJ_TIDY" != "yes"; then
    TIDY_SEARCH_DIRS=$JONJ_TIDY
  else
    TIDY_SEARCH_DIRS="/usr/local /usr"
  fi

  for i in $TIDY_SEARCH_DIRS; do
    if test -f $i/include/tidy/tidy.h; then
      TIDY_DIR=$i
      TIDY_INCDIR=$i/include/tidy
    elif test -f $i/include/tidy.h; then
      TIDY_DIR=$i
      TIDY_INCDIR=$i/include
    fi
  done

  if test -z "$TIDY_DIR"; then
    AC_MSG_ERROR(Cannot find libtidy)
  fi

  TIDY_LIBDIR=$TIDY_DIR/$JONJ_LIBDIR

  JONJ_ADD_LIBRARY_WITH_PATH(tidy, $TIDY_LIBDIR, TIDY_SHARED_LIBADD)
  JONJ_ADD_INCLUDE($TIDY_INCDIR)

  JONJ_CHECK_LIBRARY(tidy,tidyOptGetDoc,
  [
  AC_DEFINE(HAVE_TIDYOPTGETDOC,1,[ ])
  ],[],[])


  JONJ_NEW_EXTENSION(tidy, tidy.c, $ext_shared)
  JONJ_SUBST(TIDY_SHARED_LIBADD)
  AC_DEFINE(HAVE_TIDY,1,[ ])
fi
