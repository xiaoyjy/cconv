AUTOMAKE_OPTIONS = no-dependencies
EXTRA_DIST = cconv_table.h cconv_table.cc THANKS test_s2t test_t2s m4/*.m4
ACLOCAL_AMFLAGS = -I m4

AC_CFLAGS = -Wall @OS_TYPE@

bin_PROGRAMS  = cconv
cconv_SOURCES = cconv.c main.c unicode.c
cconv_CFLAGS  = ${AC_CFLAGS}
cconv_LDFLAGS = @ICONV_LIBS@ 


lib_LTLIBRARIES = libcconv.la
libcconv_la_SOURCES = cconv.c unicode.c
libcconv_la_CFLAGS = $(AC_CFLAGS)
libcconv_la_LDFLAGS = @ICONV_LIBS@ -version-info 1:0:0

include_HEADERS = cconv.h unicode.h
