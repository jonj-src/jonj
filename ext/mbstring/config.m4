dnl
dnl $Id$
dnl

AC_DEFUN([JONJ_MBSTRING_ADD_SOURCES], [
  JONJ_MBSTRING_SOURCES="$JONJ_MBSTRING_SOURCES $1"
])

AC_DEFUN([JONJ_MBSTRING_ADD_BASE_SOURCES], [
  JONJ_MBSTRING_BASE_SOURCES="$JONJ_MBSTRING_BASE_SOURCES $1"
])

AC_DEFUN([JONJ_MBSTRING_ADD_BUILD_DIR], [
  JONJ_MBSTRING_EXTRA_BUILD_DIRS="$JONJ_MBSTRING_EXTRA_BUILD_DIRS $1"
])

AC_DEFUN([JONJ_MBSTRING_ADD_INCLUDE], [
  JONJ_MBSTRING_EXTRA_INCLUDES="$JONJ_MBSTRING_EXTRA_INCLUDES $1"
])

AC_DEFUN([JONJ_MBSTRING_ADD_CONFIG_HEADER], [
  JONJ_MBSTRING_EXTRA_CONFIG_HEADERS="$JONJ_MBSTRING_EXTRA_CONFIG_HEADERS $1"
])

AC_DEFUN([JONJ_MBSTRING_ADD_CFLAG], [
  JONJ_MBSTRING_CFLAGS="$JONJ_MBSTRING_CFLAGS $1"
])

AC_DEFUN([JONJ_MBSTRING_ADD_INSTALL_HEADERS], [
  JONJ_MBSTRING_INSTALL_HEADERS="$JONJ_MBSTRING_INSTALL_HEADERS $1"
])

AC_DEFUN([JONJ_MBSTRING_EXTENSION], [
  JONJ_NEW_EXTENSION(mbstring, $JONJ_MBSTRING_SOURCES, $ext_shared,, $JONJ_MBSTRING_CFLAGS)
  JONJ_SUBST(MBSTRING_SHARED_LIBADD)

  for dir in $JONJ_MBSTRING_EXTRA_BUILD_DIRS; do
    JONJ_ADD_BUILD_DIR([$ext_builddir/$dir], 1)
  done
  
  for dir in $JONJ_MBSTRING_EXTRA_INCLUDES; do
    JONJ_ADD_INCLUDE([$ext_srcdir/$dir])
    JONJ_ADD_INCLUDE([$ext_builddir/$dir])
  done

  if test "$ext_shared" = "no"; then
    JONJ_ADD_SOURCES(JONJ_EXT_DIR(mbstring), $JONJ_MBSTRING_BASE_SOURCES)
    out="jonj_config.h"
  else
    JONJ_ADD_SOURCES_X(JONJ_EXT_DIR(mbstring),$JONJ_MBSTRING_BASE_SOURCES,,shared_objects_mbstring,yes)
    if test -f "$ext_builddir/config.h.in"; then
      out="$abs_builddir/config.h"
    else
      out="jonj_config.h"
    fi
  fi
  
  for cfg in $JONJ_MBSTRING_EXTRA_CONFIG_HEADERS; do
    cat > $ext_builddir/$cfg <<EOF
#include "$out"
EOF
  done
  JONJ_MBSTRING_ADD_INSTALL_HEADERS([mbstring.h])
  JONJ_INSTALL_HEADERS([ext/mbstring], [$JONJ_MBSTRING_INSTALL_HEADERS])
])

AC_DEFUN([JONJ_MBSTRING_SETUP_MBREGEX], [
  if test "$JONJ_MBREGEX" = "yes"; then
    if test "$JONJ_ONIG" = "yes" || test "$JONJ_ONIG" = "no"; then
      dnl
      dnl Bundled oniguruma
      dnl
      if test "$JONJ_MBREGEX_BACKTRACK" != "no"; then
        AC_DEFINE([USE_COMBINATION_EXPLOSION_CHECK],1,[whether to check multibyte regex backtrack])
      fi

      AC_CACHE_CHECK(for variable length prototypes and stdarg.h, jonj_cv_mbstring_stdarg, [
        AC_TRY_RUN([
#include <stdarg.h>
int foo(int x, ...) {
	va_list va;
	va_start(va, x);
	va_arg(va, int);
	va_arg(va, char *);
	va_arg(va, double);
	return 0;
}
int main() { return foo(10, "", 3.14); }
        ], [jonj_cv_mbstring_stdarg=yes], [jonj_cv_mbstring_stdarg=no], [
          dnl cross-compile needs something here
          case $host_alias in
          *netware*)
          jonj_cv_mbstring_stdarg=yes
          ;;
          *)
          jonj_cv_mbstring_stdarg=no
          ;;
          esac
        ])
      ])

      AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h sys/time.h sys/times.h stdarg.h])
      AC_CHECK_SIZEOF(int, 4)
      AC_CHECK_SIZEOF(short, 2)
      AC_CHECK_SIZEOF(long, 4)
      AC_C_CONST
      AC_HEADER_TIME 
      AC_FUNC_ALLOCA
      AC_FUNC_MEMCMP
      AC_CHECK_HEADER([stdarg.h], [
        AC_DEFINE([HAVE_STDARG_PROTOTYPES], [1], [Define to 1 if you have the <stdarg.h> header file.])
      ], [])
      AC_DEFINE([JONJ_ONIG_BUNDLED], [1], [Define to 1 if the bundled oniguruma is used])
      AC_DEFINE([HAVE_ONIG], [1], [Define to 1 if the oniguruma library is available]) 
      JONJ_MBSTRING_ADD_CFLAG([-DNOT_RUBY])
      JONJ_MBSTRING_ADD_BUILD_DIR([oniguruma])
      JONJ_MBSTRING_ADD_BUILD_DIR([oniguruma/enc])
      JONJ_MBSTRING_ADD_INCLUDE([oniguruma])
      JONJ_MBSTRING_ADD_CONFIG_HEADER([oniguruma/config.h])
      JONJ_MBSTRING_ADD_SOURCES([
        oniguruma/regcomp.c
        oniguruma/regerror.c
        oniguruma/regexec.c
        oniguruma/reggnu.c
        oniguruma/regparse.c
        oniguruma/regenc.c
        oniguruma/regext.c
        oniguruma/regsyntax.c
        oniguruma/regtrav.c
        oniguruma/regversion.c
        oniguruma/st.c
        oniguruma/enc/unicode.c
        oniguruma/enc/ascii.c
        oniguruma/enc/utf8.c
        oniguruma/enc/euc_jp.c
        oniguruma/enc/euc_tw.c
        oniguruma/enc/euc_kr.c
        oniguruma/enc/sjis.c
        oniguruma/enc/iso8859_1.c
        oniguruma/enc/iso8859_2.c
        oniguruma/enc/iso8859_3.c
        oniguruma/enc/iso8859_4.c
        oniguruma/enc/iso8859_5.c
        oniguruma/enc/iso8859_6.c
        oniguruma/enc/iso8859_7.c
        oniguruma/enc/iso8859_8.c
        oniguruma/enc/iso8859_9.c
        oniguruma/enc/iso8859_10.c
        oniguruma/enc/iso8859_11.c
        oniguruma/enc/iso8859_13.c
        oniguruma/enc/iso8859_14.c
        oniguruma/enc/iso8859_15.c
        oniguruma/enc/iso8859_16.c
        oniguruma/enc/koi8.c
        oniguruma/enc/koi8_r.c
        oniguruma/enc/big5.c
        oniguruma/enc/utf16_be.c
        oniguruma/enc/utf16_le.c
        oniguruma/enc/utf32_be.c
        oniguruma/enc/utf32_le.c
      ])
      JONJ_MBSTRING_ADD_INSTALL_HEADERS([oniguruma/oniguruma.h])
    else
      dnl
      dnl External oniguruma
      dnl
      if test ! -f "$JONJ_ONIG/include/oniguruma.h"; then
        AC_MSG_ERROR([oniguruma.h not found in $JONJ_ONIG/include])
      fi
      JONJ_ADD_INCLUDE([$JONJ_ONIG/include])

      JONJ_CHECK_LIBRARY(onig, onig_init, [
        JONJ_ADD_LIBRARY_WITH_PATH(onig, $JONJ_ONIG/$JONJ_LIBDIR, MBSTRING_SHARED_LIBADD)
        AC_DEFINE([HAVE_ONIG], [1], [Define to 1 if the oniguruma library is available]) 
      ],[
        AC_MSG_ERROR([Problem with oniguruma. Please check config.log for more information.])
      ], [
        -L$JONJ_ONIG/$JONJ_LIBDIR
      ])

      save_old_LDFLAGS=$LDFLAGS
      JONJ_EVAL_LIBLINE([$MBSTRING_SHARED_LIBADD], LDFLAGS)
      AC_MSG_CHECKING([if oniguruma has an invalid entry for KOI8 encoding])
      AC_TRY_LINK([
#include <oniguruma.h>
      ], [
return (int)(ONIG_ENCODING_KOI8 + 1);
      ], [
        AC_MSG_RESULT([no])
      ], [
        AC_MSG_RESULT([yes])
        AC_DEFINE([JONJ_ONIG_BAD_KOI8_ENTRY], [1], [define to 1 if oniguruma has an invalid entry for KOI8 encoding])
      ])
      LDFLAGS=$save_old_LDFLAGS
    fi

    JONJ_MBSTRING_ADD_CFLAG([-DONIG_ESCAPE_UCHAR_COLLISION=1])
    JONJ_MBSTRING_ADD_CFLAG([-DUChar=OnigUChar])

    AC_DEFINE([HAVE_MBREGEX], 1, [whether to have multibyte regex support])

    JONJ_MBSTRING_ADD_BASE_SOURCES([jonj_mbregex.c])
    JONJ_MBSTRING_ADD_INSTALL_HEADERS([jonj_mbregex.h jonj_onig_compat.h])
  fi
])

AC_DEFUN([JONJ_MBSTRING_SETUP_LIBMBFL], [
  dnl libmbfl is required and can not be disabled
  if test "$JONJ_LIBMBFL" = "yes" || test "$JONJ_LIBMBFL" = "no"; then
    dnl
    dnl Bundled libmbfl
    dnl
    JONJ_MBSTRING_ADD_BUILD_DIR([libmbfl])
    JONJ_MBSTRING_ADD_BUILD_DIR([libmbfl/mbfl])
    JONJ_MBSTRING_ADD_BUILD_DIR([libmbfl/filters])
    JONJ_MBSTRING_ADD_BUILD_DIR([libmbfl/nls])
    JONJ_MBSTRING_ADD_INCLUDE([libmbfl])
    JONJ_MBSTRING_ADD_INCLUDE([libmbfl/mbfl])
    JONJ_MBSTRING_ADD_CONFIG_HEADER([libmbfl/config.h])

    JONJ_MBSTRING_ADD_SOURCES([
     libmbfl/filters/html_entities.c
     libmbfl/filters/mbfilter_7bit.c
     libmbfl/filters/mbfilter_ascii.c
     libmbfl/filters/mbfilter_base64.c
     libmbfl/filters/mbfilter_big5.c
     libmbfl/filters/mbfilter_byte2.c
     libmbfl/filters/mbfilter_byte4.c
     libmbfl/filters/mbfilter_cp1251.c
     libmbfl/filters/mbfilter_cp1252.c
     libmbfl/filters/mbfilter_cp1254.c
     libmbfl/filters/mbfilter_cp5022x.c
     libmbfl/filters/mbfilter_cp51932.c
     libmbfl/filters/mbfilter_cp850.c
     libmbfl/filters/mbfilter_cp866.c
     libmbfl/filters/mbfilter_cp932.c
     libmbfl/filters/mbfilter_cp936.c
     libmbfl/filters/mbfilter_gb18030.c
     libmbfl/filters/mbfilter_euc_cn.c
     libmbfl/filters/mbfilter_euc_jp.c
     libmbfl/filters/mbfilter_euc_jp_2004.c
     libmbfl/filters/mbfilter_euc_jp_win.c
     libmbfl/filters/mbfilter_euc_kr.c
     libmbfl/filters/mbfilter_euc_tw.c
     libmbfl/filters/mbfilter_htmlent.c
     libmbfl/filters/mbfilter_hz.c
     libmbfl/filters/mbfilter_iso2022_jp_ms.c
     libmbfl/filters/mbfilter_iso2022jp_2004.c
     libmbfl/filters/mbfilter_iso2022jp_mobile.c
     libmbfl/filters/mbfilter_iso2022_kr.c
     libmbfl/filters/mbfilter_iso8859_1.c
     libmbfl/filters/mbfilter_iso8859_10.c
     libmbfl/filters/mbfilter_iso8859_13.c
     libmbfl/filters/mbfilter_iso8859_14.c
     libmbfl/filters/mbfilter_iso8859_15.c
     libmbfl/filters/mbfilter_iso8859_16.c
     libmbfl/filters/mbfilter_iso8859_2.c
     libmbfl/filters/mbfilter_iso8859_3.c
     libmbfl/filters/mbfilter_iso8859_4.c
     libmbfl/filters/mbfilter_iso8859_5.c
     libmbfl/filters/mbfilter_iso8859_6.c
     libmbfl/filters/mbfilter_iso8859_7.c
     libmbfl/filters/mbfilter_iso8859_8.c
     libmbfl/filters/mbfilter_iso8859_9.c
     libmbfl/filters/mbfilter_jis.c
     libmbfl/filters/mbfilter_koi8r.c
     libmbfl/filters/mbfilter_armscii8.c
     libmbfl/filters/mbfilter_qprint.c
     libmbfl/filters/mbfilter_sjis.c
     libmbfl/filters/mbfilter_sjis_open.c
     libmbfl/filters/mbfilter_sjis_mobile.c
     libmbfl/filters/mbfilter_sjis_mac.c
     libmbfl/filters/mbfilter_sjis_2004.c
     libmbfl/filters/mbfilter_tl_jisx0201_jisx0208.c
     libmbfl/filters/mbfilter_ucs2.c
     libmbfl/filters/mbfilter_ucs4.c
     libmbfl/filters/mbfilter_uhc.c
     libmbfl/filters/mbfilter_utf16.c
     libmbfl/filters/mbfilter_utf32.c
     libmbfl/filters/mbfilter_utf7.c
     libmbfl/filters/mbfilter_utf7imap.c
     libmbfl/filters/mbfilter_utf8.c
     libmbfl/filters/mbfilter_utf8_mobile.c
     libmbfl/filters/mbfilter_uuencode.c
     libmbfl/filters/mbfilter_koi8u.c
     libmbfl/mbfl/mbfilter.c
     libmbfl/mbfl/mbfilter_8bit.c
     libmbfl/mbfl/mbfilter_pass.c
     libmbfl/mbfl/mbfilter_wchar.c
     libmbfl/mbfl/mbfl_convert.c
     libmbfl/mbfl/mbfl_encoding.c
     libmbfl/mbfl/mbfl_filter_output.c
     libmbfl/mbfl/mbfl_ident.c
     libmbfl/mbfl/mbfl_language.c
     libmbfl/mbfl/mbfl_memory_device.c
     libmbfl/mbfl/mbfl_string.c
     libmbfl/mbfl/mbfl_allocators.c
     libmbfl/nls/nls_de.c
     libmbfl/nls/nls_en.c
     libmbfl/nls/nls_ja.c
     libmbfl/nls/nls_kr.c
     libmbfl/nls/nls_neutral.c
     libmbfl/nls/nls_ru.c
     libmbfl/nls/nls_uni.c
     libmbfl/nls/nls_zh.c
     libmbfl/nls/nls_hy.c
     libmbfl/nls/nls_tr.c
     libmbfl/nls/nls_ua.c
    ])
    JONJ_MBSTRING_ADD_CFLAG([-DHAVE_CONFIG_H])
    JONJ_MBSTRING_ADD_INSTALL_HEADERS([libmbfl/config.h libmbfl/mbfl/eaw_table.h libmbfl/mbfl/mbfilter.h libmbfl/mbfl/mbfilter_8bit.h libmbfl/mbfl/mbfilter_pass.h libmbfl/mbfl/mbfilter_wchar.h libmbfl/mbfl/mbfl_allocators.h libmbfl/mbfl/mbfl_consts.h libmbfl/mbfl/mbfl_convert.h libmbfl/mbfl/mbfl_defs.h libmbfl/mbfl/mbfl_encoding.h libmbfl/mbfl/mbfl_filter_output.h libmbfl/mbfl/mbfl_ident.h libmbfl/mbfl/mbfl_language.h libmbfl/mbfl/mbfl_memory_device.h libmbfl/mbfl/mbfl_string.h])
  else
    dnl
    dnl External libmfl
    dnl  
    for inc in include include/mbfl-1.0 include/mbfl; do
      if test -f "$JONJ_LIBMBFL/$inc/mbfilter.h"; then
        JONJ_LIBMBFL_INCLUDE="$inc"
        break
      fi
    done

    if test -z "$JONJ_LIBMBFL_INCLUDE"; then
      AC_MSG_ERROR([mbfilter.h not found. Please reinstall libmbfl library.])
    else 
      JONJ_ADD_INCLUDE([$JONJ_LIBMBFL_INCLUDE])
    fi

    JONJ_CHECK_LIBRARY(mbfl, mbfl_buffer_converter_new, [
      JONJ_ADD_LIBRARY_WITH_PATH(mbfl, $JONJ_LIBMBFL/$JONJ_LIBDIR, MBSTRING_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libmbfl. Please check config.log for more information.])
    ], [
      -L$JONJ_LIBMBFL/$JONJ_LIBDIR
    ])
  fi
])

dnl
dnl Main config
dnl

JONJ_ARG_ENABLE(mbstring, whether to enable multibyte string support,
[  --enable-mbstring       Enable multibyte string support])

JONJ_ARG_ENABLE([mbregex], [whether to enable multibyte regex support],
[  --disable-mbregex       MBSTRING: Disable multibyte regex support], yes, no)

JONJ_ARG_ENABLE([mbregex_backtrack], [whether to check multibyte regex backtrack],
[  --disable-mbregex-backtrack
                          MBSTRING: Disable multibyte regex backtrack check], yes, no)

JONJ_ARG_WITH(libmbfl, [for external libmbfl],
[  --with-libmbfl[=DIR]      MBSTRING: Use external libmbfl.  DIR is the libmbfl base
                          install directory [BUNDLED]], no, no)

JONJ_ARG_WITH(onig, [for external oniguruma],
[  --with-onig[=DIR]         MBSTRING: Use external oniguruma. DIR is the oniguruma install prefix.
                          If DIR is not set, the bundled oniguruma will be used], no, no)

if test "$JONJ_MBSTRING" != "no"; then  
  AC_DEFINE([HAVE_MBSTRING],1,[whether to have multibyte string support])

  JONJ_MBSTRING_ADD_BASE_SOURCES([mbstring.c jonj_unicode.c mb_gpc.c])

  if test "$JONJ_MBREGEX" != "no"; then
    JONJ_MBSTRING_SETUP_MBREGEX
  fi
  
  dnl libmbfl is required
  JONJ_MBSTRING_SETUP_LIBMBFL
  JONJ_MBSTRING_EXTENSION
fi

# vim600: sts=2 sw=2 et
