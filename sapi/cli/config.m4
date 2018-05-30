dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(cli,,
[  --disable-cli           Disable building CLI version of JONJ
                          (this forces --without-pear)], yes, no)

AC_CHECK_FUNCS(setproctitle)

AC_CHECK_HEADERS([sys/pstat.h])

AC_CACHE_CHECK([for PS_STRINGS], [cli_cv_var_PS_STRINGS],
[AC_TRY_LINK(
[#include <machine/vmparam.h>
#include <sys/exec.h>
],
[PS_STRINGS->ps_nargvstr = 1;
PS_STRINGS->ps_argvstr = "foo";],
[cli_cv_var_PS_STRINGS=yes],
[cli_cv_var_PS_STRINGS=no])])
if test "$cli_cv_var_PS_STRINGS" = yes ; then
  AC_DEFINE([HAVE_PS_STRINGS], [], [Define to 1 if the PS_STRINGS thing exists.])
fi

AC_MSG_CHECKING(for CLI build)
if test "$JONJ_CLI" != "no"; then
  JONJ_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cli/Makefile.frag)

  dnl Set filename
  SAPI_CLI_PATH=sapi/cli/jonj

  dnl Select SAPI
  JONJ_SELECT_SAPI(cli, program, jonj_cli.c jonj_http_parser.c jonj_cli_server.c ps_title.c jonj_cli_process_title.c,, '$(SAPI_CLI_PATH)')

  case $host_alias in
  *aix*)
    if test "$jonj_sapi_module" = "shared"; then
      BUILD_CLI="echo '\#! .' > jonj.sym && echo >>jonj.sym && nm -BCpg \`echo \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/.libs\/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> jonj.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:jonj.sym \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    else
      BUILD_CLI="echo '\#! .' > jonj.sym && echo >>jonj.sym && nm -BCpg \`echo \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> jonj.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:jonj.sym \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    fi
    ;;
  *darwin*)
    BUILD_CLI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(JONJ_GLOBAL_OBJS:.lo=.o) \$(JONJ_BINARY_OBJS:.lo=.o) \$(JONJ_CLI_OBJS:.lo=.o) \$(JONJ_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  *netware*)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \$(JONJ_BINARY_OBJS) \$(JONJ_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -Lnetware -ljonj5lib -o \$(SAPI_CLI_PATH)"
    ;;
  *)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \$(JONJ_GLOBAL_OBJS) \$(JONJ_BINARY_OBJS) \$(JONJ_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  esac

  dnl Set executable for tests
  JONJ_EXECUTABLE="\$(top_builddir)/\$(SAPI_CLI_PATH)"
  JONJ_SUBST(JONJ_EXECUTABLE)

  dnl Expose to Makefile
  JONJ_SUBST(SAPI_CLI_PATH)
  JONJ_SUBST(BUILD_CLI)

  JONJ_OUTPUT(sapi/cli/jonj.1)

  JONJ_INSTALL_HEADERS([sapi/cli/cli.h])
fi
AC_MSG_RESULT($JONJ_CLI)
