dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for LiteSpeed support)

JONJ_ARG_WITH(litespeed,,
[  --with-litespeed        Build JONJ as litespeed module], no)

if test "$JONJ_LITESPEED" != "no"; then
  JONJ_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/litespeed/Makefile.frag,$abs_srcdir/sapi/litespeed,sapi/litespeed)
  SAPI_LITESPEED_PATH=sapi/litespeed/jonj
  JONJ_SELECT_SAPI(litespeed, program, lsapi_main.c lsapilib.c, "", '$(SAPI_LITESPEED_PATH)') 
  case $host_alias in
  *darwin*)
    BUILD_LITESPEED="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(JONJ_GLOBAL_OBJS:.lo=.o) \$(JONJ_BINARY_OBJS:.lo=.o) \$(JONJ_LITESPEED_OBJS:.lo=.o) \$(JONJ_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_LITESPEED_PATH)"
    ;;
  *cygwin*)
    SAPI_LITESPEED_PATH=sapi/litespeed/jonj.exe
    BUILD_LITESPEED="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_LITESPEED_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_LITESPEED_PATH)"
    ;;
  *)
    BUILD_LITESPEED="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_LITESPEED_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_LITESPEED_PATH)"
    ;;
  esac

  JONJ_SUBST(SAPI_LITESPEED_PATH)
  JONJ_SUBST(BUILD_LITESPEED)
fi

AC_MSG_RESULT($JONJ_LITESPEED)
