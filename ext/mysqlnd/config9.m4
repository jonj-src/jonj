dnl
dnl $Id$
dnl config.m4 for mysqlnd driver

JONJ_ARG_ENABLE(mysqlnd, whether to enable mysqlnd,
  [  --enable-mysqlnd        Enable mysqlnd explicitly, will be done implicitly
                          when required by other extensions], no, yes)

JONJ_ARG_ENABLE(mysqlnd_compression_support, whether to disable compressed protocol support in mysqlnd,
[  --disable-mysqlnd-compression-support
                          Disable support for the MySQL compressed protocol in mysqlnd], yes, no)

if test -z "$JONJ_ZLIB_DIR"; then
  JONJ_ARG_WITH(zlib-dir, for the location of libz,
  [  --with-zlib-dir[=DIR]     mysqlnd: Set the path to libz install prefix], no, no)
fi

dnl If some extension uses mysqlnd it will get compiled in JONJ core
if test "$JONJ_MYSQLND" != "no" || test "$JONJ_MYSQLND_ENABLED" = "yes"; then
  mysqlnd_ps_sources="mysqlnd_ps.c mysqlnd_ps_codec.c"
  mysqlnd_base_sources="mysqlnd.c mysqlnd_alloc.c mysqlnd_bt.c mysqlnd_charset.c mysqlnd_wireprotocol.c \
                   mysqlnd_loaddata.c mysqlnd_reverse_api.c mysqlnd_net.c \
                   mysqlnd_statistics.c mysqlnd_driver.c mysqlnd_ext_plugin.c mysqlnd_auth.c \
				   mysqlnd_result.c mysqlnd_result_meta.c mysqlnd_debug.c\
				   mysqlnd_block_alloc.c mysqlnd_plugin.c jonj_mysqlnd.c"


  if test "$JONJ_MYSQLND_COMPRESSION_SUPPORT" != "no"; then
    AC_DEFINE([MYSQLND_COMPRESSION_WANTED], 1, [Enable compressed protocol support])
  fi

  AC_DEFINE([MYSQLND_SSL_SUPPORTED], 1, [Enable core mysqlnd SSL code])

  test -z "$JONJ_OPENSSL" && JONJ_OPENSSL=no

  if test "$JONJ_OPENSSL" != "no" || test "$JONJ_OPENSSL_DIR" != "no"; then
    AC_CHECK_LIB(ssl, DSA_get_default_method, AC_DEFINE(HAVE_DSA_DEFAULT_METHOD, 1, [OpenSSL 0.9.7 or later]))
    AC_CHECK_LIB(crypto, X509_free, AC_DEFINE(HAVE_DSA_DEFAULT_METHOD, 1, [OpenSSL 0.9.7 or later]))

    JONJ_SETUP_OPENSSL(MYSQLND_SHARED_LIBADD, [AC_DEFINE(MYSQLND_HAVE_SSL,1,[Enable mysqlnd code that uses OpenSSL directly])])
  fi

  mysqlnd_sources="$mysqlnd_base_sources $mysqlnd_ps_sources"
  JONJ_NEW_EXTENSION(mysqlnd, $mysqlnd_sources, $ext_shared)
  JONJ_ADD_BUILD_DIR([ext/mysqlnd], 1)
  JONJ_INSTALL_HEADERS([ext/mysqlnd/])
fi

if test "$JONJ_MYSQLND" != "no" || test "$JONJ_MYSQLND_ENABLED" = "yes" || test "$JONJ_MYSQLI" != "no"; then
  JONJ_ADD_BUILD_DIR([ext/mysqlnd], 1)
fi
