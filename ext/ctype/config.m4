dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(ctype, whether to enable ctype functions,
[  --disable-ctype         Disable ctype functions], yes)

if test "$JONJ_CTYPE" != "no"; then
  AC_DEFINE(HAVE_CTYPE, 1, [ ])
  JONJ_NEW_EXTENSION(ctype, ctype.c, $ext_shared)
fi
