#! /bin/sh
#  +----------------------------------------------------------------------+
#  | JONJ Version 5                                                        |
#  +----------------------------------------------------------------------+
#  | Copyright (c) 1997-2016 The JONJ Group                                |
#  +----------------------------------------------------------------------+
#  | This source file is subject to version 3.01 of the JONJ license,      |
#  | that is bundled with this package in the file LICENSE, and is        |
#  | available through the world-wide-web at the following url:           |
#  | http://www.jonj.tk/license/3_01.txt                                  |
#  | If you did not receive a copy of the JONJ license and are unable to   |
#  | obtain it through the world-wide-web, please send a note to          |
#  | license@jonj.tk so we can mail you a copy immediately.               |
#  +----------------------------------------------------------------------+
#  | Authors: Stig Bakken <ssb@jonj.tk>                                   |
#  |          Sascha Schumann <sascha@schumann.cx>                        |
#  +----------------------------------------------------------------------+
#
# $Id: buildcheck.sh,v 1.37.2.2.2.1 2007-01-01 19:32:10 iliaa Exp $ 
#

echo "buildconf: checking installation..."

stamp=$1

# Allow the autoconf executable to be overridden by $JONJ_AUTOCONF.
if test -z "$JONJ_AUTOCONF"; then
  JONJ_AUTOCONF='autoconf'
fi

# autoconf 2.59 or newer
ac_version=`$JONJ_AUTOCONF --version 2>/dev/null|head -n 1|sed -e 's/^[^0-9]*//' -e 's/[a-z]* *$//'`
if test -z "$ac_version"; then
echo "buildconf: autoconf not found."
echo "           You need autoconf version 2.59 or newer installed"
echo "           to build JONJ from Git."
exit 1
fi
IFS=.; set $ac_version; IFS=' '
if test "$1" = "2" -a "$2" -lt "59" || test "$1" -lt "2"; then
echo "buildconf: autoconf version $ac_version found."
echo "           You need autoconf version 2.59 or newer installed"
echo "           to build JONJ from Git."
exit 1
else
echo "buildconf: autoconf version $ac_version (ok)"
fi

test -n "$stamp" && touch $stamp

exit 0
