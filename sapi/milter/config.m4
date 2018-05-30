dnl
dnl $Id$
dnl

JONJ_ARG_WITH(milter, for Milter support,
[  --with-milter[=DIR]       Build JONJ as Milter application], no, no)

if test "$JONJ_MILTER" != "no"; then
  if test "$JONJ_MILTER" = "yes"; then
    if test -f /usr/lib/libmilter.a ; then
      MILTERPATH=/usr/lib
    else
      if test -f /usr/lib/libmilter/libmilter.a ; then
        MILTERPATH=/usr/lib/libmilter
      else
        AC_MSG_ERROR([Unable to find libmilter.a])
      fi
    fi
  else
    MILTERPATH=$JONJ_MILTER
  fi
  
  SAPI_MILTER_PATH=sapi/milter/jonj-milter
  JONJ_BUILD_THREAD_SAFE
  JONJ_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/milter/Makefile.frag,$abs_srcdir/sapi/milter,sapi/milter)
  JONJ_SELECT_SAPI(milter, program, jonj_milter.c getopt.c,,'$(SAPI_MILTER_PATH)') 
  JONJ_ADD_LIBRARY_WITH_PATH(milter, $MILTERPATH,)
  BUILD_MILTER="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_MILTER_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_MILTER_PATH)"
  JONJ_SUBST(SAPI_MILTER_PATH)
  JONJ_SUBST(BUILD_MILTER)
fi
