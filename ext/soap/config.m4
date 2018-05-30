dnl $Id$
dnl config.m4 for extension soap

JONJ_ARG_ENABLE(soap, whether to enable SOAP support,
[  --enable-soap           Enable SOAP support])

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir=DIR   SOAP: libxml2 install prefix], no, no)
fi

if test "$JONJ_SOAP" != "no"; then

  if test "$JONJ_LIBXML" = "no"; then   
    AC_MSG_ERROR([SOAP extension requires LIBXML extension, add --enable-libxml])                
  fi

  JONJ_SETUP_LIBXML(SOAP_SHARED_LIBADD, [
    AC_DEFINE(HAVE_SOAP,1,[ ])
    JONJ_NEW_EXTENSION(soap, soap.c jonj_encoding.c jonj_http.c jonj_packet_soap.c jonj_schema.c jonj_sdl.c jonj_xml.c, $ext_shared)
    JONJ_SUBST(SOAP_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
