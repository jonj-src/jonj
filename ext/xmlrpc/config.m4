dnl
dnl $Id$
dnl

sinclude(ext/xmlrpc/libxmlrpc/acinclude.m4)
sinclude(ext/xmlrpc/libxmlrpc/xmlrpc.m4)
sinclude(libxmlrpc/acinclude.m4)
sinclude(libxmlrpc/xmlrpc.m4)

JONJ_ARG_WITH(xmlrpc, for XMLRPC-EPI support,
[  --with-xmlrpc[=DIR]       Include XMLRPC-EPI support])

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   XMLRPC-EPI: libxml2 install prefix], no, no)
fi

JONJ_ARG_WITH(libexpat-dir, libexpat dir for XMLRPC-EPI,
[  --with-libexpat-dir=DIR XMLRPC-EPI: libexpat dir for XMLRPC-EPI (deprecated)],no,no)

JONJ_ARG_WITH(iconv-dir, iconv dir for XMLRPC-EPI,
[  --with-iconv-dir=DIR    XMLRPC-EPI: iconv dir for XMLRPC-EPI],no,no)

if test "$JONJ_XMLRPC" != "no"; then

  JONJ_ADD_EXTENSION_DEP(xmlrpc, libxml)
  JONJ_SUBST(XMLRPC_SHARED_LIBADD)
  AC_DEFINE(HAVE_XMLRPC,1,[ ])

  dnl
  dnl Default to libxml2 if --with-libexpat-dir is not used
  dnl
  if test "$JONJ_LIBEXPAT_DIR" = "no"; then

    if test "$JONJ_LIBXML" = "no"; then
      AC_MSG_ERROR([XML-RPC extension requires LIBXML extension, add --enable-libxml])
    fi

    JONJ_SETUP_LIBXML(XMLRPC_SHARED_LIBADD, [
      if test "$JONJ_XML" = "no"; then
        JONJ_ADD_SOURCES(ext/xml, compat.c)
        JONJ_ADD_BUILD_DIR(ext/xml)
      fi
    ], [
      AC_MSG_ERROR([xml2-config not found. Use --with-libxml-dir=<DIR>])
    ])
  else
    testval=no
    for i in $JONJ_LIBEXPAT_DIR $XMLRPC_DIR /usr/local /usr; do
      if test -f $i/$JONJ_LIBDIR/libexpat.a || test -f $i/$JONJ_LIBDIR/libexpat.$SHLIB_SUFFIX_NAME; then
        AC_DEFINE(HAVE_LIBEXPAT,1,[ ])
        JONJ_ADD_LIBRARY_WITH_PATH(expat, $i/$JONJ_LIBDIR, XMLRPC_SHARED_LIBADD)
        JONJ_ADD_INCLUDE($i/include)
        testval=yes
        break
      fi
    done

    if test "$testval" = "no"; then
      AC_MSG_ERROR([XML-RPC support requires libexpat. Use --with-libexpat-dir=<DIR> (deprecated!)])
    fi
  fi

  dnl if iconv is shared or missing then we should build iconv ourselves
  if test "$JONJ_ICONV_SHARED" = "yes" || test "$JONJ_ICONV" = "no"; then

    if test "$JONJ_ICONV_DIR" != "no"; then
      JONJ_ICONV=$JONJ_ICONV_DIR
    fi
  
    if test -z "$JONJ_ICONV" || test "$JONJ_ICONV" = "no"; then
      JONJ_ICONV=yes
    fi
  
    JONJ_SETUP_ICONV(XMLRPC_SHARED_LIBADD, [], [
      AC_MSG_ERROR([iconv not found, in order to build xmlrpc you need the iconv library])
    ])
  fi
fi

if test "$JONJ_XMLRPC" = "yes"; then
  XMLRPC_CHECKS
  JONJ_NEW_EXTENSION(xmlrpc,xmlrpc-epi-jonj.c libxmlrpc/base64.c \
          libxmlrpc/simplestring.c libxmlrpc/xml_to_dandarpc.c \
          libxmlrpc/xmlrpc_introspection.c libxmlrpc/encodings.c \
          libxmlrpc/system_methods.c libxmlrpc/xml_to_xmlrpc.c \
          libxmlrpc/queue.c libxmlrpc/xml_element.c libxmlrpc/xmlrpc.c \
          libxmlrpc/xml_to_soap.c,$ext_shared,,
          -I@ext_srcdir@/libxmlrpc -DVERSION="0.50")
  JONJ_ADD_BUILD_DIR($ext_builddir/libxmlrpc)
  XMLRPC_MODULE_TYPE=builtin

elif test "$JONJ_XMLRPC" != "no"; then

  if test -r $JONJ_XMLRPC/include/xmlrpc.h; then
    XMLRPC_DIR=$JONJ_XMLRPC/include
  elif test -r $JONJ_XMLRPC/include/xmlrpc-epi/xmlrpc.h; then
dnl some xmlrpc-epi header files have generic file names like
dnl queue.h or base64.h. Distributions have to create dir
dnl for xmlrpc-epi because of this.
    XMLRPC_DIR=$JONJ_XMLRPC/include/xmlrpc-epi
  else
    AC_MSG_CHECKING(for XMLRPC-EPI in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/xmlrpc.h; then
        XMLRPC_DIR=$i/include
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$XMLRPC_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the XMLRPC-EPI distribution)
  fi

  JONJ_ADD_INCLUDE($XMLRPC_DIR)
  JONJ_ADD_LIBRARY_WITH_PATH(xmlrpc, $XMLRPC_DIR/$JONJ_LIBDIR, XMLRPC_SHARED_LIBADD)
  JONJ_NEW_EXTENSION(xmlrpc,xmlrpc-epi-jonj.c, $ext_shared)
  XMLRPC_MODULE_TYPE=external
fi
