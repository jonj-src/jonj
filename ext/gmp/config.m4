dnl
dnl $Id$
dnl

JONJ_ARG_WITH(gmp, for GNU MP support,
[  --with-gmp[=DIR]          Include GNU MP support])

if test "$JONJ_GMP" != "no"; then

  for i in $JONJ_GMP /usr/local /usr; do
    test -f $i/include/gmp.h && GMP_DIR=$i && break
  done

  if test -z "$GMP_DIR"; then
    AC_MSG_ERROR(Unable to locate gmp.h)
  fi
 
  JONJ_CHECK_LIBRARY(gmp, __gmp_randinit_lc_2exp_size,
  [],[
    JONJ_CHECK_LIBRARY(gmp, gmp_randinit_lc_2exp_size,
    [],[
      AC_MSG_ERROR([GNU MP Library version 4.1.2 or greater required.])
    ],[
      -L$GMP_DIR/$JONJ_LIBDIR
    ])
  ],[
    -L$GMP_DIR/$JONJ_LIBDIR
  ])

  JONJ_ADD_LIBRARY_WITH_PATH(gmp, $GMP_DIR/$JONJ_LIBDIR, GMP_SHARED_LIBADD)
  JONJ_ADD_INCLUDE($GMP_DIR/include)

  JONJ_NEW_EXTENSION(gmp, gmp.c, $ext_shared)
  JONJ_SUBST(GMP_SHARED_LIBADD)
  AC_DEFINE(HAVE_GMP, 1, [ ])
fi
