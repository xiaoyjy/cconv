AUTOMAKE_OPTIONS = no-dependencies

EXTRA_DIST = cconv_table.h cconv_table.cc m4/*.m4 THANKS test_s2t test_t2s

AC_CFLAGS = -Wall @ICONV_INCLUDES@ @OS_TYPE@
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS  = cconv
cconv_SOURCES = cconv.c main.c unicode.c
cconv_CFLAGS  = ${AC_CFLAGS}
cconv_LDFLAGS = @ICONV_LIBS@

lib_LTLIBRARIES = libcconv.la
libcconv_la_SOURCES = cconv.c unicode.c
libcconv_la_CFLAGS  = -Wall @ICONV_INCLUDES@ @OS_TYPE@

include_HEADERS = cconv.h unicode.h
