dnl $Id$
JONJ_ARG_ENABLE(shmop, whether to enable shmop support, 
[  --enable-shmop          Enable shmop support])

if test "$JONJ_SHMOP" != "no"; then
  AC_DEFINE(HAVE_SHMOP, 1, [ ])
  JONJ_NEW_EXTENSION(shmop, shmop.c, $ext_shared)
fi
