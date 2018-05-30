dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(libxml, whether to enable LIBXML support,
[  --disable-libxml        Disable LIBXML support], yes)

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir[=DIR]   LIBXML: libxml2 install prefix], no, no)
fi

if test "$JONJ_LIBXML" != "no"; then

  dnl This extension can not be build as shared
  ext_shared=no

  JONJ_SETUP_LIBXML(LIBXML_SHARED_LIBADD, [
    AC_DEFINE(HAVE_LIBXML,1,[ ])
    JONJ_NEW_EXTENSION(libxml, [libxml.c], $ext_shared)
    JONJ_INSTALL_HEADERS([ext/libxml/jonj_libxml.h])
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
