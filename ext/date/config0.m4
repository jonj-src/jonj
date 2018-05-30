dnl $Id$
dnl config.m4 for date extension

sinclude(ext/date/lib/timelib.m4)
sinclude(lib/timelib.m4)

JONJ_DATE_CFLAGS="-I@ext_builddir@/lib -D HAVE_TIMELIB_CONFIG_H=1"
timelib_sources="lib/astro.c lib/dow.c lib/parse_date.c lib/parse_tz.c 
                 lib/timelib.c lib/tm2unixtime.c lib/unixtime2tm.c lib/parse_iso_intervals.c lib/interval.c"

JONJ_NEW_EXTENSION(date, jonj_date.c $timelib_sources, no,, $JONJ_DATE_CFLAGS)

JONJ_ADD_BUILD_DIR([$ext_builddir/lib], 1)
JONJ_ADD_INCLUDE([$ext_builddir/lib])
JONJ_ADD_INCLUDE([$ext_srcdir/lib])

JONJ_INSTALL_HEADERS([ext/date], [jonj_date.h lib/timelib.h lib/timelib_structs.h lib/timelib_config.h])
AC_DEFINE([HAVE_TIMELIB_CONFIG_H], [1], [Have timelib_config.h])

cat > $ext_builddir/lib/timelib_config.h <<EOF
#ifdef JONJ_WIN32
# include "config.w32.h"
#else
# include <jonj_config.h>
#endif
#include <jonj_stdint.h>
#define TIMELIB_OMIT_STDINT 1
EOF
