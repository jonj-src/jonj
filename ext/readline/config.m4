dnl
dnl $Id$
dnl

JONJ_ARG_WITH(libedit,for libedit readline replacement, 
[  --with-libedit[=DIR]      Include libedit readline replacement (CLI/CGI only)])

if test "$JONJ_LIBEDIT" = "no"; then
  JONJ_ARG_WITH(readline,for readline support,
  [  --with-readline[=DIR]     Include readline support (CLI/CGI only)])
else
  dnl "register" the --with-readline option to preven invalid "unknown configure option" warning
  jonj_with_readline=no
fi

if test "$JONJ_READLINE" && test "$JONJ_READLINE" != "no"; then
  for i in $JONJ_READLINE /usr/local /usr; do
    test -f $i/include/readline/readline.h && READLINE_DIR=$i && break
  done

  if test -z "$READLINE_DIR"; then
    AC_MSG_ERROR(Please reinstall readline - I cannot find readline.h)
  fi

  JONJ_ADD_INCLUDE($READLINE_DIR/include)

  JONJ_READLINE_LIBS=""
  AC_CHECK_LIB(ncurses, tgetent,
  [
    JONJ_ADD_LIBRARY(ncurses,,READLINE_SHARED_LIBADD)
    JONJ_READLINE_LIBS="$JONJ_READLINE_LIBS -lncurses"
  ],[
    AC_CHECK_LIB(termcap, tgetent,
    [
      JONJ_ADD_LIBRARY(termcap,,READLINE_SHARED_LIBADD)
      JONJ_READLINE_LIBS="$JONJ_READLINE_LIBS -ltermcap"
    ])
  ])

  JONJ_CHECK_LIBRARY(readline, readline,
  [
    JONJ_ADD_LIBRARY_WITH_PATH(readline, $READLINE_DIR/$JONJ_LIBDIR, READLINE_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR(readline library not found)
  ], [
    -L$READLINE_DIR/$JONJ_LIBDIR $JONJ_READLINE_LIBS
  ])

  JONJ_CHECK_LIBRARY(readline, rl_pending_input,
  [], [
    AC_MSG_ERROR([invalid readline installation detected. Try --with-libedit instead.])
  ], [
    -L$READLINE_DIR/$JONJ_LIBDIR $JONJ_READLINE_LIBS
  ])

  JONJ_CHECK_LIBRARY(readline, rl_callback_read_char,
  [
    AC_DEFINE(HAVE_RL_CALLBACK_READ_CHAR, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$JONJ_LIBDIR $JONJ_READLINE_LIBS
  ])

  JONJ_CHECK_LIBRARY(edit, rl_on_new_line,
  [
    AC_DEFINE(HAVE_RL_ON_NEW_LINE, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$JONJ_LIBDIR $JONJ_READLINE_LIBS
  ])

  AC_DEFINE(HAVE_LIBREADLINE, 1, [ ])

elif test "$JONJ_LIBEDIT" != "no"; then

  for i in $JONJ_LIBEDIT /usr/local /usr; do
    test -f $i/include/editline/readline.h && LIBEDIT_DIR=$i && break
  done

  if test -z "$LIBEDIT_DIR"; then
    AC_MSG_ERROR(Please reinstall libedit - I cannot find readline.h)
  fi

  JONJ_ADD_INCLUDE($LIBEDIT_DIR/include)

  AC_CHECK_LIB(ncurses, tgetent,
  [
    JONJ_ADD_LIBRARY(ncurses,,READLINE_SHARED_LIBADD)
  ],[
    AC_CHECK_LIB(termcap, tgetent,
    [
      JONJ_ADD_LIBRARY(termcap,,READLINE_SHARED_LIBADD)
    ])
  ])

  JONJ_CHECK_LIBRARY(edit, readline,
  [
    JONJ_ADD_LIBRARY_WITH_PATH(edit, $LIBEDIT_DIR/$JONJ_LIBDIR, READLINE_SHARED_LIBADD)  
  ], [
    AC_MSG_ERROR(edit library required by readline not found)
  ], [
    -L$READLINE_DIR/$JONJ_LIBDIR 
  ])

  JONJ_CHECK_LIBRARY(edit, rl_callback_read_char,
  [
    AC_DEFINE(HAVE_RL_CALLBACK_READ_CHAR, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$JONJ_LIBDIR
  ])

  JONJ_CHECK_LIBRARY(edit, rl_on_new_line,
  [
    AC_DEFINE(HAVE_RL_ON_NEW_LINE, 1, [ ])
  ],[],[
    -L$READLINE_DIR/$JONJ_LIBDIR
  ])

  AC_DEFINE(HAVE_LIBEDIT, 1, [ ])
fi

if test "$JONJ_READLINE" != "no" || test "$JONJ_LIBEDIT" != "no"; then
  AC_CHECK_FUNCS([rl_completion_matches])
  JONJ_NEW_EXTENSION(readline, readline.c readline_cli.c, $ext_shared, cli)
  JONJ_SUBST(READLINE_SHARED_LIBADD)
fi
