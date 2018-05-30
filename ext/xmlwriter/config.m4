dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(xmlwriter, whether to enable XMLWriter support,
[  --disable-xmlwriter     Disable XMLWriter support], yes)

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   XMLWriter: libxml2 install prefix], no, no)
fi

if test "$JONJ_XMLWRITER" != "no"; then

  if test "$JONJ_LIBXML" = "no"; then
    AC_MSG_ERROR([XMLWriter extension requires LIBXML extension, add --enable-libxml])
  fi

  JONJ_SETUP_LIBXML(XMLWRITER_SHARED_LIBADD, [
    AC_DEFINE(HAVE_XMLWRITER,1,[ ])
    JONJ_NEW_EXTENSION(xmlwriter, jonj_xmlwriter.c, $ext_shared)
    JONJ_SUBST(XMLWRITER_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
