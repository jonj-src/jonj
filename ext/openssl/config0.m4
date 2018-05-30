dnl
dnl $Id$
dnl

JONJ_ARG_WITH(openssl, for OpenSSL support,
[  --with-openssl[=DIR]      Include OpenSSL support (requires OpenSSL >= 0.9.6)])

JONJ_ARG_WITH(kerberos, for Kerberos support,
[  --with-kerberos[=DIR]     OPENSSL: Include Kerberos support], no, no)

JONJ_ARG_WITH(system-ciphers, whether to use system default cipher list instead of hardcoded value,
[  --with-system-ciphers   OPENSSL: Use system default cipher list instead of hardcoded value], no, no)

if test "$JONJ_OPENSSL" != "no"; then
  JONJ_NEW_EXTENSION(openssl, openssl.c xp_ssl.c, $ext_shared)
  JONJ_SUBST(OPENSSL_SHARED_LIBADD)

  if test "$JONJ_KERBEROS" != "no"; then
    JONJ_SETUP_KERBEROS(OPENSSL_SHARED_LIBADD)
  fi

  AC_CHECK_LIB(ssl, DSA_get_default_method, AC_DEFINE(HAVE_DSA_DEFAULT_METHOD, 1, [OpenSSL 0.9.7 or later]))
  AC_CHECK_LIB(crypto, X509_free, AC_DEFINE(HAVE_DSA_DEFAULT_METHOD, 1, [OpenSSL 0.9.7 or later]))
  AC_CHECK_FUNCS([RAND_egd])

  JONJ_SETUP_OPENSSL(OPENSSL_SHARED_LIBADD, 
  [
    AC_DEFINE(HAVE_OPENSSL_EXT,1,[ ])
  ], [
    AC_MSG_ERROR([OpenSSL check failed. Please check config.log for more information.])
  ])
  if test "$JONJ_SYSTEM_CIPHERS" != "no"; then
    AC_DEFINE(USE_OPENSSL_SYSTEM_CIPHERS,1,[ Use system default cipher list instead of hardcoded value ])
  fi
fi
