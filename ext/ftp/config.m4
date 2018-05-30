dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(ftp,whether to enable FTP support,
[  --enable-ftp            Enable FTP support])

JONJ_ARG_WITH(openssl-dir,OpenSSL dir for FTP,
[  --with-openssl-dir[=DIR]  FTP: openssl install prefix], no, no)

if test "$JONJ_FTP" = "yes"; then
  AC_DEFINE(HAVE_FTP,1,[Whether you want FTP support])
  JONJ_NEW_EXTENSION(ftp, jonj_ftp.c ftp.c, $ext_shared)

  dnl Empty variable means 'no'
  test -z "$JONJ_OPENSSL" && JONJ_OPENSSL=no

  if test "$JONJ_OPENSSL" != "no" || test "$JONJ_OPENSSL_DIR" != "no"; then
    JONJ_SETUP_OPENSSL(FTP_SHARED_LIBADD)
    JONJ_SUBST(FTP_SHARED_LIBADD)
  fi
fi
