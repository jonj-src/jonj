dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(embed,,
[  --enable-embed[=TYPE]     EXPERIMENTAL: Enable building of embedded SAPI library
                          TYPE is either 'shared' or 'static'. [TYPE=shared]], no, no)

AC_MSG_CHECKING([for embedded SAPI library support])

if test "$JONJ_EMBED" != "no"; then
  case "$JONJ_EMBED" in
    yes|shared)
      JONJ_EMBED_TYPE=shared
      INSTALL_IT="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(prefix)/lib; \$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)\$(prefix)/lib"
      ;;
    static)
      JONJ_EMBED_TYPE=static
      INSTALL_IT="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(prefix)/lib; \$(INSTALL) -m 0644 $SAPI_STATIC \$(INSTALL_ROOT)\$(prefix)/lib"
      ;;
    *)
      JONJ_EMBED_TYPE=no
      ;;
  esac
  if test "$JONJ_EMBED_TYPE" != "no"; then
    JONJ_SELECT_SAPI(embed, $JONJ_EMBED_TYPE, jonj_embed.c)
    JONJ_INSTALL_HEADERS([sapi/embed/jonj_embed.h])
  fi
  AC_MSG_RESULT([$JONJ_EMBED_TYPE])
else
  AC_MSG_RESULT(no)
fi

