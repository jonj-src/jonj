dnl $Id$
dnl config.m4 for extension tokenizer

dnl Otherwise use enable:

JONJ_ARG_ENABLE(tokenizer, whether to enable tokenizer support,
[  --disable-tokenizer     Disable tokenizer support], yes)

if test "$JONJ_TOKENIZER" != "no"; then
  JONJ_NEW_EXTENSION(tokenizer, tokenizer.c tokenizer_data.c, $ext_shared)
  JONJ_ADD_MAKEFILE_FRAGMENT
fi
