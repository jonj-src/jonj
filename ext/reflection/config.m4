dnl $Id$
dnl config.m4 for extension reflection

AC_DEFINE(HAVE_REFLECTION, 1, [Whether Reflection is enabled])
JONJ_NEW_EXTENSION(reflection, jonj_reflection.c, no)
