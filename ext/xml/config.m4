dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(xml,whether to enable XML support,
[  --disable-xml           Disable XML support], yes)

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   XML: libxml2 install prefix], no, no)
fi

JONJ_ARG_WITH(libexpat-dir, libexpat install dir,
[  --with-libexpat-dir=DIR XML: libexpat install prefix (deprecated)], no, no)

if test "$JONJ_XML" != "no"; then

  dnl 
  dnl Default to libxml2 if --with-libexpat-dir is not used.
  dnl
  if test "$JONJ_LIBEXPAT_DIR" = "no"; then

    if test "$JONJ_LIBXML" = "no"; then
      AC_MSG_ERROR([XML extension requires LIBXML extension, add --enable-libxml])
    fi

    JONJ_SETUP_LIBXML(XML_SHARED_LIBADD, [
      xml_extra_sources="compat.c"
      JONJ_ADD_EXTENSION_DEP(xml, libxml)
    ], [
      AC_MSG_ERROR([xml2-config not found. Use --with-libxml-dir=<DIR>])
    ])
  fi
  
  dnl
  dnl Check for expat only if --with-libexpat-dir is used.
  dnl
  if test "$JONJ_LIBEXPAT_DIR" != "no"; then
    for i in $JONJ_XML $JONJ_LIBEXPAT_DIR /usr /usr/local; do
      if test -f "$i/$JONJ_LIBDIR/libexpat.a" || test -f "$i/$JONJ_LIBDIR/libexpat.$SHLIB_SUFFIX_NAME"; then
        EXPAT_DIR=$i
        break
      fi
    done

    if test -z "$EXPAT_DIR"; then
      AC_MSG_ERROR([not found. Please reinstall the expat distribution.])
    fi

    JONJ_ADD_INCLUDE($EXPAT_DIR/include)
    JONJ_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/$JONJ_LIBDIR, XML_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
  fi

  JONJ_NEW_EXTENSION(xml, xml.c $xml_extra_sources, $ext_shared)
  JONJ_SUBST(XML_SHARED_LIBADD)
  JONJ_INSTALL_HEADERS([ext/xml/])
  AC_DEFINE(HAVE_XML, 1, [ ])
fi
