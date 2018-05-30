dnl
dnl $Id$
dnl

JONJ_ARG_ENABLE(jonjdbg, for jonjdbg support,
[  --enable-jonjdbg         Build jonjdbg], no, no)

JONJ_ARG_ENABLE(jonjdbg-debug, for jonjdbg debug build,
[  --enable-jonjdbg-debug   Build jonjdbg in debug mode], no, no)

if test "$JONJ_JONJDBG" != "no"; then
  AC_HEADER_TIOCGWINSZ
  AC_DEFINE(HAVE_JONJDBG, 1, [ ])

  if test "$JONJ_JONJDBG_DEBUG" != "no"; then
    AC_DEFINE(JONJDBG_DEBUG, 1, [ ])
  else
    AC_DEFINE(JONJDBG_DEBUG, 0, [ ])
  fi

  JONJ_JONJDBG_CFLAGS="-D_GNU_SOURCE"
  JONJ_JONJDBG_FILES="jonjdbg.c jonjdbg_parser.c jonjdbg_lexer.c jonjdbg_prompt.c jonjdbg_help.c jonjdbg_break.c jonjdbg_print.c jonjdbg_bp.c jonjdbg_opcode.c jonjdbg_list.c jonjdbg_utils.c jonjdbg_info.c jonjdbg_cmd.c jonjdbg_set.c jonjdbg_frame.c jonjdbg_watch.c jonjdbg_btree.c"

  if test "$JONJ_READLINE" != "no" -o  "$JONJ_LIBEDIT" != "no"; then
  	JONJDBG_EXTRA_LIBS="$JONJ_READLINE_LIBS"
  fi
  
  JONJ_SUBST(JONJ_JONJDBG_CFLAGS)
  JONJ_SUBST(JONJ_JONJDBG_FILES)
  JONJ_SUBST(JONJDBG_EXTRA_LIBS)
  
  JONJ_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/jonjdbg/Makefile.frag], [$abs_srcdir/sapi/jonjdbg], [$abs_builddir/sapi/jonjdbg])
  JONJ_SELECT_SAPI(jonjdbg, program, $JONJ_JONJDBG_FILES, $JONJ_JONJDBG_CFLAGS, [$(SAPI_JONJDBG_PATH)])

  BUILD_BINARY="sapi/jonjdbg/jonjdbg"
  BUILD_SHARED="sapi/jonjdbg/libjonjdbg.la"
  
  BUILD_JONJDBG="\$(LIBTOOL) --mode=link \
        \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \
                \$(JONJ_GLOBAL_OBJS) \
                \$(JONJ_BINARY_OBJS) \
                \$(JONJ_JONJDBG_OBJS) \
                \$(EXTRA_LIBS) \
                \$(JONJDBG_EXTRA_LIBS) \
                \$(ZEND_EXTRA_LIBS) \
         -o \$(BUILD_BINARY)"

  BUILD_JONJDBG_SHARED="\$(LIBTOOL) --mode=link \
        \$(CC) -shared -Wl,-soname,libjonjdbg.so -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(JONJ_RPATHS) \
                \$(JONJ_GLOBAL_OBJS) \
                \$(JONJ_BINARY_OBJS) \
                \$(JONJ_JONJDBG_OBJS) \
                \$(EXTRA_LIBS) \
                \$(JONJDBG_EXTRA_LIBS) \
                \$(ZEND_EXTRA_LIBS) \
                \-DJONJDBG_SHARED \
         -o \$(BUILD_SHARED)"

  JONJ_SUBST(BUILD_BINARY)
  JONJ_SUBST(BUILD_SHARED)
  JONJ_SUBST(BUILD_JONJDBG)
  JONJ_SUBST(BUILD_JONJDBG_SHARED)
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
