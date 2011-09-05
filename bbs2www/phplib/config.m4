dnl $Id: config.m4.in 8522 2005-12-28 06:48:28Z atppp $
dnl config.m4 for extension kbs_bbs

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(kbs_bbs, for kbs_bbs support,
dnl Make sure that the comment is aligned:
dnl [  --with-kbs_bbs             Include kbs_bbs support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(kbs_bbs, whether to enable kbs_bbs support,
Make sure that the comment is aligned:
[  --enable-kbs_bbs           Enable kbs_bbs support])

if test "$PHP_KBS_BBS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-kbs_bbs -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/kbs_bbs.h"  # you most likely want to change this
  dnl if test -r $PHP_KBS_BBS/; then # path given as parameter
  dnl   KBS_BBS_DIR=$PHP_KBS_BBS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for kbs_bbs files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       KBS_BBS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$KBS_BBS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the kbs_bbs distribution])
  dnl fi

  dnl # --with-kbs_bbs -> add include path
  PHP_ADD_INCLUDE(@WWWSRCDIR@/../src)
  PHP_ADD_INCLUDE("@WWWSRCDIR@/include")

  dnl # --with-kbs_bbs -> chech for lib and symbol presence
  dnl LIBNAME=kbs_bbs # you may want to change this
  dnl LIBSYMBOL=kbs_bbs # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $KBS_BBS_DIR/lib, KBS_BBS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_KBS_BBSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong kbs_bbs lib version or lib not found])
  dnl ],[
  dnl   -L$KBS_BBS_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl # --with-kbs_bbs -> chech for lib and symbol presence

  PHP_ADD_LIBRARY_WITH_PATH(system,@WWWSRCDIR@/../libsystem,KBS_BBS_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(BBS,@WWWSRCDIR@/../libBBS,KBS_BBS_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(bbslib,@WWWSRCDIR@/lib,KBS_BBS_SHARED_LIBADD)
  PHP_SUBST(KBS_BBS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(kbs_bbs, phpbbslib.c, $ext_shared)
fi
