dnl $Id$
dnl config.m4 for extension iplookup

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

 PHP_ARG_WITH(iplookup, for iplookup support,
 Make sure that the comment is aligned:
 [  --with-iplookup             Include iplookup support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(iplookup, whether to enable iplookup support,
dnl Make sure that the comment is aligned:
dnl [  --enable-iplookup           Enable iplookup support])

if test "$PHP_IPLOOKUP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-iplookup -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/iplookup.h"  # you most likely want to change this
  dnl if test -r $PHP_IPLOOKUP/$SEARCH_FOR; then # path given as parameter
  dnl   IPLOOKUP_DIR=$PHP_IPLOOKUP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for iplookup files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       IPLOOKUP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$IPLOOKUP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the iplookup distribution])
  dnl fi

  dnl # --with-iplookup -> add include path
  dnl PHP_ADD_INCLUDE($IPLOOKUP_DIR/include)

  dnl # --with-iplookup -> check for lib and symbol presence
  dnl LIBNAME=iplookup # you may want to change this
  dnl LIBSYMBOL=iplookup # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $IPLOOKUP_DIR/$PHP_LIBDIR, IPLOOKUP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_IPLOOKUPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong iplookup lib version or lib not found])
  dnl ],[
  dnl   -L$IPLOOKUP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(IPLOOKUP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(iplookup, iplookup.c, $ext_shared)
fi
