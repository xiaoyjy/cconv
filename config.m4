AC_DEFINE(PACKAGE_VERSION, "0.6.4")
AC_DEFINE(PACKAGE_NAME, "cconv")
AC_DEFINE(PACKAGE_BUGREPORT, "reg256@sina.com.cn")

PHP_ARG_ENABLE(cconv, whether to enable cconv support,
[ --enable-cconv   Enable cconv support])

if test -z "$PHP_CCONV_DIR"; then
  PHP_ARG_WITH(cconv-dir, for the location of libcconv,
  [  --with-cconv-dir[=DIR]   Set the path to cconv install prefix], no, no)
fi

AC_DEFUN([PHP_CCONV_BIN],[
  if test "$PHP_CCONV_DIR" == "no"; then
    PHP_CCONV_DIR=""
  fi

  for i in $PHP_CCONV_DIR /usr/local/cconv /usr/local /usr; do
    (test -f $i/$PHP_LIBDIR/libcconv.$SHLIB_SUFFIX_NAME ||\
     test -f $i/$PHP_LIBDIR/libcconv.a) && PHP_CCONV_DIR=$i && break
  done

  if test -z "$PHP_CCONV_DIR"; then
    AC_MSG_RESULT([If configure fails try --with-cconv-dir=<DIR>])
    AC_MSG_ERROR([libcconv.(a|so) not found.])
  fi

  PHP_ADD_INCLUDE($PHP_CCONV_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(cconv, $PHP_CCONV_DIR/$PHP_LIBDIR, CCONV_SHARED_LIBADD)
])

CFLAGS="$CFLAGS -D"`uname -s`

if test "$PHP_CCONV" = "yes"; then
  PHP_CCONV_BIN
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, "", EXTRA_LDFLAGS)
  PHP_NEW_EXTENSION(cconv, php_cconv.c, $ext_shared)
  PHP_SUBST(CCONV_SHARED_LIBADD)
fi
