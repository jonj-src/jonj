dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(session, whether to enable JONJ sessions,
[  --disable-session       Disable session support], yes)

JONJ_ARG_WITH(mm,for mm support,
[  --with-mm[=DIR]           SESSION: Include mm support for session storage], no, no)

if test "$JONJ_SESSION" != "no"; then
  JONJ_PWRITE_TEST
  JONJ_PREAD_TEST
  JONJ_NEW_EXTENSION(session, mod_user_class.c session.c mod_files.c mod_mm.c mod_user.c, $ext_shared)
  JONJ_ADD_EXTENSION_DEP(session, hash, true)
  JONJ_ADD_EXTENSION_DEP(session, spl)
  JONJ_SUBST(SESSION_SHARED_LIBADD)
  JONJ_INSTALL_HEADERS(ext/session, [jonj_session.h mod_files.h mod_user.h])
  AC_DEFINE(HAVE_JONJ_SESSION,1,[ ])
fi

if test "$JONJ_MM" != "no"; then
  for i in $JONJ_MM /usr/local /usr; do
    test -f "$i/include/mm.h" && MM_DIR=$i && break
  done

  if test -z "$MM_DIR" ; then
    AC_MSG_ERROR(cannot find mm library)
  fi
  
  JONJ_ADD_LIBRARY_WITH_PATH(mm, $MM_DIR/$JONJ_LIBDIR, SESSION_SHARED_LIBADD)
  JONJ_ADD_INCLUDE($MM_DIR/include)
  JONJ_INSTALL_HEADERS([ext/session/mod_mm.h])
  AC_DEFINE(HAVE_LIBMM, 1, [Whether you have libmm])
fi
