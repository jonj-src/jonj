dnl
dnl $Id$
dnl

dnl Check for extensions with which Recode can not work
if test "$JONJ_RECODE" != "no"; then
  test "$JONJ_IMAP"  != "no" && recode_conflict="$recode_conflict imap"

  if test -n "$MYSQL_LIBNAME"; then
    JONJ_CHECK_LIBRARY($MYSQL_LIBNAME, hash_insert, [
      recode_conflict="$recode_conflict mysql"
    ])
  fi

  if test -n "$recode_conflict"; then
    AC_MSG_ERROR([recode extension can not be configured together with:$recode_conflict])
  fi
fi
