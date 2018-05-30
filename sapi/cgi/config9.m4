dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(cgi,,
[  --disable-cgi           Disable building CGI version of JONJ], yes, no)

dnl
dnl CGI setup
dnl
AC_MSG_CHECKING(for CGI build)
if test "$JONJ_CGI" != "no"; then
    AC_MSG_RESULT(yes)
    AC_MSG_CHECKING([for socklen_t in sys/socket.h])
    AC_EGREP_HEADER([socklen_t], [sys/socket.h],
      [AC_MSG_RESULT([yes])
       AC_DEFINE([HAVE_SOCKLEN_T], [1],
        [Define if the socklen_t typedef is in sys/socket.h])],
      AC_MSG_RESULT([no]))

    AC_MSG_CHECKING([for sun_len in sys/un.h])
    AC_EGREP_HEADER([sun_len], [sys/un.h],
      [AC_MSG_RESULT([yes])
       AC_DEFINE([HAVE_SOCKADDR_UN_SUN_LEN], [1],
        [Define if sockaddr_un in sys/un.h contains a sun_len component])],
      AC_MSG_RESULT([no]))

    AC_MSG_CHECKING([whether cross-process locking is required by accept()])
    case "`uname -sr`" in
      IRIX\ 5.* | SunOS\ 5.* | UNIX_System_V\ 4.0)	
        AC_MSG_RESULT([yes])
        AC_DEFINE([USE_LOCKING], [1], 
          [Define if cross-process locking is required by accept()])
      ;;
      *)
        AC_MSG_RESULT([no])
      ;;
    esac

    JONJ_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cgi/Makefile.frag)

    dnl Set filename
    case $host_alias in
      *cygwin* )
        SAPI_CGI_PATH=sapi/cgi/jonj-cgi.exe
        ;;
      * )
        SAPI_CGI_PATH=sapi/cgi/jonj-cgi
        ;;
    esac

    dnl Select SAPI
    JONJ_SELECT_SAPI(cgi, program, cgi_main.c fastcgi.c,, '$(SAPI_CGI_PATH)')

    case $host_alias in
      *aix*)
        if test "$jonj_sapi_module" = "shared"; then
          BUILD_CGI="echo '\#! .' > jonj.sym && echo >>jonj.sym && nm -BCpg \`echo \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CGI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/.libs\/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> jonj.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:jonj.sym \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CGI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
        else
          BUILD_CGI="echo '\#! .' > jonj.sym && echo >>jonj.sym && nm -BCpg \`echo \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CGI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> jonj.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:jonj.sym \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CGI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
        fi
        ;;
      *darwin*)
        BUILD_CGI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(JONJ_GLOBAL_OBJS:.lo=.o) \$(JONJ_BINARY_OBJS:.lo=.o) \$(JONJ_CGI_OBJS:.lo=.o) \$(JONJ_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
      *)
        BUILD_CGI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CGI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CGI_PATH)"
      ;;
    esac

    dnl Expose to Makefile
    JONJ_SUBST(SAPI_CGI_PATH)
    JONJ_SUBST(BUILD_CGI)

    JONJ_OUTPUT(sapi/cgi/jonj-cgi.1)
else
  AC_MSG_RESULT(yes)
fi
