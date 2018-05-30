dnl $Id$
dnl config.m4 for extension simplexml

JONJ_ARG_ENABLE(simplexml, whether to enable SimpleXML support,
[  --disable-simplexml     Disable SimpleXML support], yes)

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   SimpleXML: libxml2 install prefix], no, no)
fi

if test "$JONJ_SIMPLEXML" != "no"; then

  if test "$JONJ_LIBXML" = "no"; then   
    AC_MSG_ERROR([SimpleXML extension requires LIBXML extension, add --enable-libxml])                
  fi

  JONJ_SETUP_LIBXML(SIMPLEXML_SHARED_LIBADD, [
    AC_DEFINE(HAVE_SIMPLEXML,1,[ ])
    JONJ_NEW_EXTENSION(simplexml, simplexml.c sxe.c, $ext_shared)
    JONJ_INSTALL_HEADERS([ext/simplexml/jonj_simplexml.h ext/simplexml/jonj_simplexml_exports.h])
    JONJ_SUBST(SIMPLEXML_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
  JONJ_ADD_EXTENSION_DEP(simplexml, libxml)
  JONJ_ADD_EXTENSION_DEP(simplexml, spl, true)
fi
