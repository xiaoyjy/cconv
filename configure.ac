#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.72])
AC_INIT([cconv],[0.6.4],[xiaoyjy@gmail.com])
AC_CONFIG_SRCDIR([cconv.c])
AC_CONFIG_HEADERS([config.h])

AC_COPYRIGHT([Copyright (C) 2008, 2009 Yang Jianyu])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
LT_INIT
AM_PROG_AR

# Checks for header files.
m4_warn([obsolete],
[The preprocessor macro 'STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  C89 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS([stdlib.h string.h unistd.h wchar.h])
AC_CHECK_HEADERS([iconv.h], [], [AC_MSG_ERROR("iconv.h header not found - please install libiconv")])

# Checks iconv lib.
AC_CHECK_FUNCS(iconv_open, [], [
  AC_CHECK_LIB(iconv, iconv_open, [
    ICONV_LIBS="-liconv"
  ], [
    AC_MSG_ERROR("iconv library not found")
  ])
])
AC_SUBST(ICONV_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h memory.h stdint.h stdlib.h strings.h string.h sys/stat.h sys/types.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memcpy memset strncasecmp])

# echo $host_os; may use host_os here.
OS_TYPE="-D"`uname -s`
AC_SUBST(OS_TYPE)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
