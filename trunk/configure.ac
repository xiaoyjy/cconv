#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.13)
AC_INIT(cconv, 0.5.3, xiaoyjy@gmail.com)
AM_INIT_AUTOMAKE(cconv, 0.5.3)
AC_CONFIG_SRCDIR([cconv.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# echo $host_os; may use host_os here.
OS_TYPE="-D"`uname -s`
AC_SUBST(OS_TYPE)

for i in /usr/local/iconv /usr/local /usr; do
  (test -f $i/lib/libiconv.so ||\
   test -f $i/lib/libiconv.a) && ICONV_DIR=$i && break
done

if test -z "$ICONV_DIR"; then
  ICONV_LIBS=""
  ICONV_INCLUDES=""
else
  ICONV_LIBS="-L$ICONV_DIR/lib -liconv"
  ICONV_INCLUDES="-I$ICONV_DIR/include"
fi

AC_SUBST(ICONV_INCLUDES)
AC_SUBST(ICONV_LIBS)

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([memset strncasecmp])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
