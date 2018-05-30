dnl
dnl $Id$ 
dnl

JONJ_ARG_WITH(curl, for cURL support,
[  --with-curl[=DIR]         Include cURL support])

if test "$JONJ_CURL" != "no"; then
  if test -r $JONJ_CURL/include/curl/easy.h; then
    CURL_DIR=$JONJ_CURL
  else
    AC_MSG_CHECKING(for cURL in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/curl/easy.h; then
        CURL_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$CURL_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libcurl distribution -
    easy.h should be in <curl-dir>/include/curl/)
  fi

  CURL_CONFIG="curl-config"
  AC_MSG_CHECKING(for cURL 7.10.5 or greater)

  if ${CURL_DIR}/bin/curl-config --libs > /dev/null 2>&1; then
    CURL_CONFIG=${CURL_DIR}/bin/curl-config
  else
    if ${CURL_DIR}/curl-config --libs > /dev/null 2>&1; then
      CURL_CONFIG=${CURL_DIR}/curl-config
    fi
  fi

  curl_version_full=`$CURL_CONFIG --version`
  curl_version=`echo ${curl_version_full} | sed -e 's/libcurl //' | $AWK 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
  if test "$curl_version" -ge 7010005; then
    AC_MSG_RESULT($curl_version_full)
    CURL_LIBS=`$CURL_CONFIG --libs`
  else
    AC_MSG_ERROR(cURL version 7.10.5 or later is required to compile jonj with cURL support)
  fi

  JONJ_ADD_INCLUDE($CURL_DIR/include)
  JONJ_EVAL_LIBLINE($CURL_LIBS, CURL_SHARED_LIBADD)
  JONJ_ADD_LIBRARY_WITH_PATH(curl, $CURL_DIR/$JONJ_LIBDIR, CURL_SHARED_LIBADD)
  
  AC_MSG_CHECKING([for SSL support in libcurl])
  CURL_SSL=`$CURL_CONFIG --feature | $EGREP SSL`
  if test "$CURL_SSL" = "SSL"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_CURL_SSL], [1], [Have cURL with  SSL support])
   
    save_CFLAGS="$CFLAGS"
    CFLAGS="`$CURL_CONFIG --cflags`"
   
    AC_PROG_CPP
    AC_MSG_CHECKING([for openssl support in libcurl])
    AC_TRY_RUN([
#include <curl/curl.h>

int main(int argc, char *argv[])
{
  curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);

  if (data && data->ssl_version && *data->ssl_version) {
    const char *ptr = data->ssl_version;

    while(*ptr == ' ') ++ptr;
    return strncasecmp(ptr, "OpenSSL", sizeof("OpenSSL")-1);
  }
  return 1;
}
    ],[
      AC_MSG_RESULT([yes])
      AC_CHECK_HEADERS([openssl/crypto.h], [
        AC_DEFINE([HAVE_CURL_OPENSSL], [1], [Have cURL with OpenSSL support])
      ])
    ], [
      AC_MSG_RESULT([no])
    ], [
      AC_MSG_RESULT([no])
    ])
   
    AC_MSG_CHECKING([for gnutls support in libcurl])
    AC_TRY_RUN([
#include <curl/curl.h>

int main(int argc, char *argv[])
{
  curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);
  
  if (data && data->ssl_version && *data->ssl_version) {
    const char *ptr = data->ssl_version;

    while(*ptr == ' ') ++ptr;
    return strncasecmp(ptr, "GnuTLS", sizeof("GnuTLS")-1);
  }
  return 1;
}
], [
      AC_MSG_RESULT([yes])
      AC_CHECK_HEADER([gcrypt.h], [
        AC_DEFINE([HAVE_CURL_GNUTLS], [1], [Have cURL with GnuTLS support])
      ])
    ], [
      AC_MSG_RESULT([no])
    ], [
      AC_MSG_RESULT([no])
    ])
   
    CFLAGS="$save_CFLAGS"
  else
    AC_MSG_RESULT([no])
  fi

  JONJ_CHECK_LIBRARY(curl,curl_easy_perform, 
  [ 
    AC_DEFINE(HAVE_CURL,1,[ ])
  ],[
    AC_MSG_ERROR(There is something wrong. Please check config.log for more information.)
  ],[
    $CURL_LIBS -L$CURL_DIR/$JONJ_LIBDIR
  ])

  JONJ_CHECK_LIBRARY(curl,curl_easy_strerror,
  [
    AC_DEFINE(HAVE_CURL_EASY_STRERROR,1,[ ])
  ],[],[
    $CURL_LIBS -L$CURL_DIR/$JONJ_LIBDIR
  ])

  JONJ_CHECK_LIBRARY(curl,curl_multi_strerror,
  [
    AC_DEFINE(HAVE_CURL_MULTI_STRERROR,1,[ ])
  ],[],[
    $CURL_LIBS -L$CURL_DIR/$JONJ_LIBDIR
  ])

  JONJ_NEW_EXTENSION(curl, interface.c multi.c share.c curl_file.c, $ext_shared)
  JONJ_SUBST(CURL_SHARED_LIBADD)
fi
