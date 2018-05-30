dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(wddx,whether to enable WDDX support,
[  --enable-wddx           Enable WDDX support])

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   WDDX: libxml2 install prefix], no, no)
fi

JONJ_ARG_WITH(libexpat-dir, libexpat dir for WDDX,
[  --with-libexpat-dir=DIR WDDX: libexpat dir for XMLRPC-EPI (deprecated)],no,no)

if test "$JONJ_WDDX" != "no"; then

  dnl
  dnl Default to libxml2 if --with-libexpat-dir is not used
  dnl
  if test "$JONJ_LIBEXPAT_DIR" = "no"; then
    if test "$JONJ_LIBXML" = "no"; then
      AC_MSG_ERROR([WDDX extension requires LIBXML extension, add --enable-libxml])
    fi

    JONJ_SETUP_LIBXML(WDDX_SHARED_LIBADD, [
      if test "$JONJ_XML" = "no"; then
        JONJ_ADD_SOURCES(ext/xml, compat.c)
        JONJ_ADD_BUILD_DIR(ext/xml)
      fi
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
    JONJ_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/$JONJ_LIBDIR, WDDX_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
  fi

  AC_DEFINE(HAVE_WDDX, 1, [ ])
  JONJ_NEW_EXTENSION(wddx, wddx.c, $ext_shared)
  JONJ_ADD_EXTENSION_DEP(wddx, libxml)
  JONJ_SUBST(XMLRPC_SHARED_LIBADD)
fi
