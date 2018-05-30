dnl
dnl $Id$
dnl

JONJ_ARG_WITH(xsl, for XSL support,
[  --with-xsl[=DIR]          Include XSL support.  DIR is the libxslt base
                          install directory (libxslt >= 1.1.0 required)])

if test "$JONJ_XSL" != "no"; then

  if test "$JONJ_LIBXML" = "no"; then
    AC_MSG_ERROR([XSL extension requires LIBXML extension, add --enable-libxml])
  fi
  
  if test "$JONJ_DOM" = "no"; then
    AC_MSG_ERROR([XSL extension requires DOM extension, add --enable-dom])
  fi

  for i in $JONJ_XSL /usr/local /usr; do
    if test -x "$i/bin/xslt-config"; then
      XSLT_CONFIG=$i/bin/xslt-config
      break
    fi
  done

  if test -z "$XSLT_CONFIG"; then
    AC_MSG_ERROR([xslt-config not found. Please reinstall the libxslt >= 1.1.0 distribution])
  else
    libxslt_full_version=`$XSLT_CONFIG --version`
    ac_IFS=$IFS
    IFS="."
    set $libxslt_full_version
    IFS=$ac_IFS
    LIBXSLT_VERSION=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`
    if test "$LIBXSLT_VERSION" -ge "1001000"; then
      XSL_LIBS=`$XSLT_CONFIG --libs`
      XSL_INCS=`$XSLT_CONFIG --cflags`
      JONJ_EVAL_LIBLINE($XSL_LIBS, XSL_SHARED_LIBADD)
      JONJ_EVAL_INCLINE($XSL_INCS)
      
      AC_MSG_CHECKING([for EXSLT support])
      for i in $JONJ_XSL /usr/local /usr; do
        if test -r "$i/include/libexslt/exslt.h"; then
          JONJ_XSL_EXSL_DIR=$i
          break
        fi
      done
      if test -z "$JONJ_XSL_EXSL_DIR"; then
        AC_MSG_RESULT(not found)
      else 
        AC_MSG_RESULT(found)
        JONJ_ADD_LIBRARY_WITH_PATH(exslt, $JONJ_XSL_EXSL_DIR/$JONJ_LIBDIR, XSL_SHARED_LIBADD)
        JONJ_ADD_INCLUDE($JONJ_XSL_EXSL_DIR/include)
        AC_DEFINE(HAVE_XSL_EXSLT,1,[ ])
      fi
    else
      AC_MSG_ERROR([libxslt version 1.1.0 or greater required.])
    fi
    
  
  fi
  
  AC_DEFINE(HAVE_XSL,1,[ ])
  JONJ_NEW_EXTENSION(xsl, jonj_xsl.c xsltprocessor.c, $ext_shared)
  JONJ_SUBST(XSL_SHARED_LIBADD)
  JONJ_ADD_EXTENSION_DEP(xsl, libxml)
fi
