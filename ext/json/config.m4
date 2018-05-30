dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(json, whether to enable JavaScript Object Serialization support,
[  --disable-json          Disable JavaScript Object Serialization support], yes)

if test "$JONJ_JSON" != "no"; then
  AC_DEFINE([HAVE_JSON],1 ,[whether to enable JavaScript Object Serialization support])
  AC_HEADER_STDC

  JONJ_NEW_EXTENSION(json, json.c JSON_parser.c, $ext_shared)
  JONJ_INSTALL_HEADERS([ext/json], [jonj_json.h])
  JONJ_SUBST(JSON_SHARED_LIBADD)
fi
