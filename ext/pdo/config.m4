dnl $Id$
dnl config.m4 for extension pdo
dnl vim:se ts=2 sw=2 et:

AC_DEFUN([JONJ_PDO_PEAR_CHECK],[
  pdo_running_under_pear=0
  case `pwd` in
    /var/tmp/pear-build-*)
      pdo_running_under_pear=1
      ;;
  esac

  if test "$pdo_running_under_pear$JONJ_PEAR_VERSION" = "1"; then
    # we're running in an environment that smells like pear,
    # and the JONJ_PEAR_VERSION env var is not set.  That implies
    # that we're running under a slightly broken pear installer
    AC_MSG_ERROR([
PDO requires that you upgrade your PEAR installer tools. Please
do so now by running:

  % sudo pear upgrade pear

or by manually downloading and installing PEAR version 1.3.5 or higher.

Once you've upgraded, please re-try your PDO install.
    ])
  fi
])

JONJ_ARG_ENABLE(pdo, whether to enable PDO support,
[  --disable-pdo           Disable JONJ Data Objects support], yes)

if test "$JONJ_PDO" != "no"; then

  dnl Make sure $JONJ_PDO is 'yes' when it's not 'no' :)
  JONJ_PDO=yes

  JONJ_PDO_PEAR_CHECK

  if test "$ext_shared" = "yes" ; then
    case $host_alias in
      *darwin*)
          AC_MSG_ERROR([
Due to the way that loadable modules work on OSX/Darwin, you need to
compile the PDO package statically into the JONJ core.

Please follow the instructions at: http://netevil.org/node.jonj?nid=202
for more detail on this issue.
          ])
        ext_shared=no
        ;;
    esac
  fi
  JONJ_NEW_EXTENSION(pdo, pdo.c pdo_dbh.c pdo_stmt.c pdo_sql_parser.c pdo_sqlstate.c, $ext_shared)
  ifdef([JONJ_ADD_EXTENSION_DEP],
  [
    JONJ_ADD_EXTENSION_DEP(pdo, spl, true)
  ])
  
  ifdef([JONJ_INSTALL_HEADERS],
  [
    dnl Sadly, this is a complete NOP for pecl extensions
    JONJ_INSTALL_HEADERS(ext/pdo, [jonj_pdo.h jonj_pdo_driver.h jonj_pdo_error.h])
  ])

  dnl so we always include the known-good working hack.
  JONJ_ADD_MAKEFILE_FRAGMENT
fi

