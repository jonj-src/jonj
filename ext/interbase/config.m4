JONJ_ARG_WITH(interbase,for InterBase support,
[  --with-interbase[=DIR]    Include InterBase support.  DIR is the InterBase base
                          install directory [/usr/interbase]])

if test "$JONJ_INTERBASE" != "no"; then
  if test "$JONJ_INTERBASE" = "yes"; then
    IBASE_INCDIR=/usr/interbase/include
    IBASE_LIBDIR=/usr/interbase/lib
  else
    IBASE_INCDIR=$JONJ_INTERBASE/include
    IBASE_LIBDIR=$JONJ_INTERBASE/$JONJ_LIBDIR
  fi

  JONJ_CHECK_LIBRARY(fbclient, isc_detach_database,
  [
    IBASE_LIBNAME=fbclient
  ], [
    JONJ_CHECK_LIBRARY(gds, isc_detach_database,
    [
      IBASE_LIBNAME=gds
    ], [
      JONJ_CHECK_LIBRARY(ib_util, isc_detach_database,
      [
        IBASE_LIBNAME=ib_util
      ], [
        AC_MSG_ERROR([libgds, libib_util or libfbclient not found! Check config.log for more information.])
      ], [
        -L$IBASE_LIBDIR
      ])
    ], [
      -L$IBASE_LIBDIR
    ])
  ], [
    -L$IBASE_LIBDIR
  ])
  
  JONJ_ADD_LIBRARY_WITH_PATH($IBASE_LIBNAME, $IBASE_LIBDIR, INTERBASE_SHARED_LIBADD)
  JONJ_ADD_INCLUDE($IBASE_INCDIR)
  AC_DEFINE(HAVE_IBASE,1,[ ])
  JONJ_NEW_EXTENSION(interbase, interbase.c ibase_query.c ibase_service.c ibase_events.c ibase_blobs.c, $ext_shared)
  JONJ_SUBST(INTERBASE_SHARED_LIBADD)
fi
