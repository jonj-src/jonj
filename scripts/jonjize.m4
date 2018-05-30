dnl This file becomes configure.in for self-contained extensions.

AC_PREREQ(2.59)
AC_INIT(config.m4)
ifdef([AC_PRESERVE_HELP_ORDER], [AC_PRESERVE_HELP_ORDER], [])

JONJ_CONFIG_NICE(config.nice)

dnl 
AC_DEFUN([JONJ_EXT_BUILDDIR],[.])dnl
AC_DEFUN([JONJ_EXT_DIR],[""])dnl
AC_DEFUN([JONJ_EXT_SRCDIR],[$abs_srcdir])dnl
AC_DEFUN([JONJ_ALWAYS_SHARED],[
  ext_output="yes, shared"
  ext_shared=yes
  test "[$]$1" = "no" && $1=yes
])dnl
dnl

test -z "$CFLAGS" && auto_cflags=1

abs_srcdir=`(cd $srcdir && pwd)`
abs_builddir=`pwd`

AC_PROG_CC([cc gcc])
JONJ_DETECT_ICC
JONJ_DETECT_SUNCC
AC_PROG_CC_C_O

dnl Support systems with system libraries in e.g. /usr/lib64
JONJ_ARG_WITH(libdir, for system library directory,
[  --with-libdir=NAME      Look for libraries in .../NAME rather than .../lib], lib, no)

JONJ_RUNPATH_SWITCH
JONJ_SHLIB_SUFFIX_NAMES

dnl Find jonj-config script
JONJ_ARG_WITH(jonj-config,,
[  --with-jonj-config=PATH  Path to jonj-config [jonj-config]], jonj-config, no)

dnl For BC
JONJ_CONFIG=$JONJ_JONJ_CONFIG
prefix=`$JONJ_CONFIG --prefix 2>/dev/null`
jonjincludedir=`$JONJ_CONFIG --include-dir 2>/dev/null`
INCLUDES=`$JONJ_CONFIG --includes 2>/dev/null`
EXTENSION_DIR=`$JONJ_CONFIG --extension-dir 2>/dev/null`
JONJ_EXECUTABLE=`$JONJ_CONFIG --jonj-binary 2>/dev/null`
 
if test -z "$prefix"; then
  AC_MSG_ERROR([Cannot find jonj-config. Please use --with-jonj-config=PATH])
fi

jonj_shtool=$srcdir/build/shtool
JONJ_INIT_BUILD_SYSTEM

AC_MSG_CHECKING([for JONJ prefix])
AC_MSG_RESULT([$prefix])
AC_MSG_CHECKING([for JONJ includes])
AC_MSG_RESULT([$INCLUDES])
AC_MSG_CHECKING([for JONJ extension directory])
AC_MSG_RESULT([$EXTENSION_DIR])
AC_MSG_CHECKING([for JONJ installed headers prefix])
AC_MSG_RESULT([$jonjincludedir])

dnl Checks for JONJ_DEBUG / ZEND_DEBUG / ZTS
AC_MSG_CHECKING([if debug is enabled])
old_CPPFLAGS=$CPPFLAGS
CPPFLAGS="-I$jonjincludedir"
AC_EGREP_CPP(jonj_debug_is_enabled,[
#include <main/jonj_config.h>
#if ZEND_DEBUG
jonj_debug_is_enabled
#endif
],[
  JONJ_DEBUG=yes
],[
  JONJ_DEBUG=no
])
CPPFLAGS=$old_CPPFLAGS
AC_MSG_RESULT([$JONJ_DEBUG])

AC_MSG_CHECKING([if zts is enabled])
old_CPPFLAGS=$CPPFLAGS
CPPFLAGS="-I$jonjincludedir"
AC_EGREP_CPP(jonj_zts_is_enabled,[
#include <main/jonj_config.h>
#if ZTS
jonj_zts_is_enabled
#endif
],[
  JONJ_THREAD_SAFETY=yes
],[
  JONJ_THREAD_SAFETY=no
])
CPPFLAGS=$old_CPPFLAGS
AC_MSG_RESULT([$JONJ_DEBUG])

dnl Support for building and testing Zend extensions
ZEND_EXT_TYPE="zend_extension"
JONJ_SUBST(ZEND_EXT_TYPE)

dnl Discard optimization flags when debugging is enabled
if test "$JONJ_DEBUG" = "yes"; then
  JONJ_DEBUG=1
  ZEND_DEBUG=yes
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  changequote([,])
  dnl add -O0 only if GCC or ICC is used
  if test "$GCC" = "yes" || test "$ICC" = "yes"; then
    CFLAGS="$CFLAGS -O0"
    CXXFLAGS="$CXXFLAGS -g -O0"
  fi
  if test "$SUNCC" = "yes"; then
    if test -n "$auto_cflags"; then
      CFLAGS="-g"
      CXXFLAGS="-g"
    else
      CFLAGS="$CFLAGS -g"
      CXXFLAGS="$CFLAGS -g"
    fi
  fi
else
  JONJ_DEBUG=0
  ZEND_DEBUG=no
fi

dnl Always shared
JONJ_BUILD_SHARED

dnl Required programs
JONJ_PROG_RE2C
JONJ_PROG_AWK

sinclude(config.m4)

enable_static=no
enable_shared=yes

dnl Only allow AC_PROG_CXX and AC_PROG_CXXCPP if they are explicitly called (by JONJ_REQUIRE_CXX).
dnl Otherwise AC_PROG_LIBTOOL fails if there is no working C++ compiler.
AC_PROVIDE_IFELSE([JONJ_REQUIRE_CXX], [], [
  undefine([AC_PROG_CXX])
  AC_DEFUN([AC_PROG_CXX], [])
  undefine([AC_PROG_CXXCPP])
  AC_DEFUN([AC_PROG_CXXCPP], [jonj_prog_cxxcpp=disabled])
])
AC_PROG_LIBTOOL

all_targets='$(JONJ_MODULES) $(JONJ_ZEND_EX)'
install_targets="install-modules install-headers"
jonjlibdir="`pwd`/modules"
CPPFLAGS="$CPPFLAGS -DHAVE_CONFIG_H"
CFLAGS_CLEAN='$(CFLAGS)'
CXXFLAGS_CLEAN='$(CXXFLAGS)'

test "$prefix" = "NONE" && prefix="/usr/local"
test "$exec_prefix" = "NONE" && exec_prefix='$(prefix)'

JONJ_SUBST(JONJ_MODULES)
JONJ_SUBST(JONJ_ZEND_EX)

JONJ_SUBST(all_targets)
JONJ_SUBST(install_targets)

JONJ_SUBST(prefix)
JONJ_SUBST(exec_prefix)
JONJ_SUBST(libdir)
JONJ_SUBST(prefix)
JONJ_SUBST(jonjlibdir)
JONJ_SUBST(jonjincludedir)

JONJ_SUBST(CC)
JONJ_SUBST(CFLAGS)
JONJ_SUBST(CFLAGS_CLEAN)
JONJ_SUBST(CPP)
JONJ_SUBST(CPPFLAGS)
JONJ_SUBST(CXX)
JONJ_SUBST(CXXFLAGS)
JONJ_SUBST(CXXFLAGS_CLEAN)
JONJ_SUBST(EXTENSION_DIR)
JONJ_SUBST(JONJ_EXECUTABLE)
JONJ_SUBST(EXTRA_LDFLAGS)
JONJ_SUBST(EXTRA_LIBS)
JONJ_SUBST(INCLUDES)
JONJ_SUBST(LFLAGS)
JONJ_SUBST(LDFLAGS)
JONJ_SUBST(SHARED_LIBTOOL)
JONJ_SUBST(LIBTOOL)
JONJ_SUBST(SHELL)
JONJ_SUBST(INSTALL_HEADERS)

JONJ_GEN_BUILD_DIRS
JONJ_GEN_GLOBAL_MAKEFILE

test -d modules || $jonj_shtool mkdir modules
touch .deps

AC_CONFIG_HEADER(config.h)

AC_OUTPUT()
