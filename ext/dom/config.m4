dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(dom, whether to enable DOM support,
[  --disable-dom           Disable DOM support], yes)

if test -z "$JONJ_LIBXML_DIR"; then
  JONJ_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir[=DIR]   DOM: libxml2 install prefix], no, no)
fi

if test "$JONJ_DOM" != "no"; then

  if test "$JONJ_LIBXML" = "no"; then   
    AC_MSG_ERROR([DOM extension requires LIBXML extension, add --enable-libxml])
  fi

  JONJ_SETUP_LIBXML(DOM_SHARED_LIBADD, [
    AC_DEFINE(HAVE_DOM,1,[ ])
    JONJ_NEW_EXTENSION(dom, [jonj_dom.c attr.c document.c domerrorhandler.c \
                            domstringlist.c domexception.c namelist.c \
                            processinginstruction.c cdatasection.c \
                            documentfragment.c domimplementation.c \
                            element.c node.c string_extend.c characterdata.c \
                            documenttype.c domimplementationlist.c entity.c \
                            nodelist.c text.c comment.c domconfiguration.c \
                            domimplementationsource.c entityreference.c \
                            notation.c xpath.c dom_iterators.c \
                            typeinfo.c domerror.c domlocator.c namednodemap.c userdatahandler.c], 
                            $ext_shared)
    JONJ_SUBST(DOM_SHARED_LIBADD)
    JONJ_INSTALL_HEADERS([ext/dom/xml_common.h])
    JONJ_ADD_EXTENSION_DEP(dom, libxml)
  ], [
    AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
  ])
fi
