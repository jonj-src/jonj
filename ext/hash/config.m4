dnl $Id$
dnl config.m4 for extension hash

JONJ_ARG_WITH(mhash, for mhash support,
[  --with-mhash[=DIR]        Include mhash support])

JONJ_ARG_ENABLE(hash, whether to enable hash support,
[  --disable-hash          Disable hash support], yes)

if test "$JONJ_MHASH" != "no"; then
  if test "$JONJ_HASH" = "no"; then
    JONJ_HASH="yes"
  fi

  AC_DEFINE(JONJ_MHASH_BC, 1, [ ])
fi

if test "$JONJ_HASH" != "no"; then
  AC_DEFINE(HAVE_HASH_EXT,1,[Have HASH Extension])

  JONJ_C_BIGENDIAN

  AC_CHECK_SIZEOF(short, 2)
  AC_CHECK_SIZEOF(int, 4)
  AC_CHECK_SIZEOF(long, 4)
  AC_CHECK_SIZEOF(long long, 8)
  
  EXT_HASH_SOURCES="hash.c hash_md.c hash_sha.c hash_ripemd.c hash_haval.c \
    hash_tiger.c hash_gost.c hash_snefru.c hash_whirlpool.c hash_adler32.c \
    hash_crc32.c hash_fnv.c hash_joaat.c"
  EXT_HASH_HEADERS="jonj_hash.h jonj_hash_md.h jonj_hash_sha.h jonj_hash_ripemd.h \
    jonj_hash_haval.h jonj_hash_tiger.h jonj_hash_gost.h jonj_hash_snefru.h \
    jonj_hash_whirlpool.h jonj_hash_adler32.h jonj_hash_crc32.h \
    jonj_hash_fnv.h jonj_hash_joaat.h"
  
  JONJ_NEW_EXTENSION(hash, $EXT_HASH_SOURCES, $ext_shared)
  ifdef([JONJ_INSTALL_HEADERS], [
  	JONJ_INSTALL_HEADERS(ext/hash, $EXT_HASH_HEADERS)
  ])
fi
