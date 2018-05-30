dnl $Id$
dnl config.m4 for extension phar

JONJ_ARG_ENABLE(phar, for phar archive support,
[  --disable-phar          Disable phar support], yes)

if test "$JONJ_PHAR" != "no"; then
  JONJ_NEW_EXTENSION(phar, util.c tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared)
  AC_MSG_CHECKING([for phar openssl support])
  if test "$JONJ_HASH_SHARED" != "yes"; then
    if test "$JONJ_HASH" != "no"; then
      AC_DEFINE(PHAR_HASH_OK,1,[ ])
    fi
  else
    AC_MSG_WARN([Phar: sha256/sha512 signature support disabled if ext/hash is built shared])
  fi
  if test "$JONJ_OPENSSL_SHARED" = "yes"; then
    AC_MSG_RESULT([no (shared openssl)])
  else
    if test "$JONJ_OPENSSL" = "yes"; then
      AC_MSG_RESULT([yes])
      AC_DEFINE(PHAR_HAVE_OPENSSL,1,[ ])
    else
      AC_MSG_RESULT([no])
    fi
  fi
  JONJ_ADD_EXTENSION_DEP(phar, hash, true)
  JONJ_ADD_EXTENSION_DEP(phar, spl, true)
  JONJ_ADD_MAKEFILE_FRAGMENT

  JONJ_INSTALL_HEADERS([ext/phar], [jonj_phar.h])

  JONJ_OUTPUT(ext/phar/phar.1 ext/phar/phar.phar.1)
fi
