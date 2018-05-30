dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(xmlreader, whether to enable XMLReader support,
[  --disable-xmlreader     Disable XMLReader support], yes)

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   XMLReader: libxml2 install prefix], no, no)
fi

if test "$JONJ_XMLREADER" != "no"; then

  if test "$JONJ_LIBXML" = "no"; then
    AC_MSG_ERROR([XMLReader extension requires LIBXML extension, add --enable-libxml])
  fi

  JONJ_SETUP_LIBXML(XMLREADER_SHARED_LIBADD, [
    AC_DEFINE(HAVE_XMLREADER,1,[ ])
    JONJ_NEW_EXTENSION(xmlreader, jonj_xmlreader.c, $ext_shared)
    JONJ_ADD_EXTENSION_DEP(xmlreader, dom, true)
    JONJ_SUBST(XMLREADER_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
