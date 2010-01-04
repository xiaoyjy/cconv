/*
 * Copyright (C) 2008, 2009
 * Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * \author Yang Jianyu <xiaoyjy@hotmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#include "cconv.h"

#define CCONV_BUFFER_SIZE 4096
#define CCONV_STRLEN_SIZE 256

#ifdef HAVE_CONFIG_H
	#include "config.h"
#else
	#define PACKAGE_VERSION "0.0.1"
	#define PACKAGE_BUGREPORT "xiaoyjy@gmail.com"
#endif

/* {{{ void usage( char *s_program ) */
/**
 * Output usage.
 *
 * @param   s_program   Program command line.
 * @return			  alway = 0.
 */
static void usage(char *s_program)
{
	char *p = (char *)rindex(s_program, '/');
	printf("\n");
	printf("Chinese-Convert Tool. Version %s (inside libcconv version %s).\n",
			PACKAGE_VERSION, PACKAGE_VERSION);
	printf("Copyright (c) 2008-2009, China. %s \n", PACKAGE_BUGREPORT);
	printf("\n" );
	printf("Usage: %s [OPTION...] [FILE]\n", (p ? p+1 : s_program));
	printf("\n" );
	printf("Convert encoding of given files from one encoding to another.\n");
	printf("\n" );
	printf(" Input/Output format specification:\n");
	printf("  -f [NAME] encoding of original text! at this you can use GBK, BIG5 or UTF8\n");
	printf("  -t [NAME] encoding for output! at this you can use GBK, BIG5, UTF8-CN, UTF8-HK or UTF-TW\n");
	printf("\n" );
	printf(" Information:\n" );
	printf("  -l list coded character sets can used \n");
	printf("\n");
	printf(" Output control:\n");
	printf("  -o [FILE] output file\n");
	printf("\n");
	printf("  -?, -h, -v  Show this help page.\n");
	printf("Report bugs to <%s>\n", PACKAGE_BUGREPORT);
}
/* }}} */


/* {{{ void code_list( void ) */
/**
 * Output encodings are supported.
 *
 * @return			  alway = 0.
 */
void code_list( void )
{
	printf("\n");
	printf("The following encodings are supported:\n");
	printf("\n");
	printf("European languages:\n");
	printf("   ASCII\n");
	printf("Chinese:\n");
	printf("   GB2312, GBK, GB-HANS,GB-HANT ,GB18030, BIG5, UTF8, UTF8-CN, UTF8-TW, UTF8-HK\n");
	printf("Other:\n");
	printf("   All encoding supported by iconv.\n");
	printf("\n");
}
/* }}} */

const char* filter_code(const char* code)
{
	if(strcasecmp("UTF8", code) == 0)
		return CCONV_CODE_UTF;

	if(  strcasecmp("GB2312", code) == 0
          || strcasecmp("GBK", code) == 0)
		return CCONV_CODE_GBL;

	if(  strcasecmp("UTF8-CN" , code) == 0
	  || strcasecmp("UTF-8-CN", code) == 0)
		return CCONV_CODE_UHS;

	if(  strcasecmp("UTF8-TW" , code) == 0
	  || strcasecmp("UTF-8-TW", code) == 0
	  || strcasecmp("UTF8-HK" , code) == 0
	  || strcasecmp("UTF-8-HK", code) == 0)
		return CCONV_CODE_UHT;

	if(  strcasecmp("BIG5"     , code) == 0
          || strcasecmp("BIG-5"    , code) == 0
          || strcasecmp("CN-BIG5"  , code) == 0
          || strcasecmp("BIG5HKSCS", code) == 0
          || strcasecmp("BIG-FIVE" , code) == 0
          || strcasecmp("BIGFIVE"  , code) == 0)
		return CCONV_CODE_BIG;

	return code;
}

/**
 * Main function.
 */
int main(int argc, char *argv[])
{
	FILE   *fp_in  = NULL,
	       *fp_out = NULL;

	char   *inbuf, *outbuf, *ps_inbuf, *ps_outbuf;
	cconv_t conv;
	const char *tocode   = NULL,
		   *fromcode = NULL;
	size_t inlen, outlen, ret, i_left, inbuf_size, outbuf_size;
	int    ch = 0;

	inbuf  = (char*)malloc(CCONV_BUFFER_SIZE);
	outbuf = (char*)malloc(CCONV_BUFFER_SIZE*2);
	ps_inbuf  = inbuf;
	ps_outbuf = outbuf;
	while ((ch = getopt(argc, argv, "f:t:i:o:l?hv")) != -1) {
		switch (ch) {
		case 'f':
			fromcode = optarg;
			break;
		case 't':
			tocode   = optarg;
			break;
		case 'i':
			if((fp_in = fopen(optarg, "r")) == NULL)
			{
				printf("%s: '%s' %s\n", argv[0], optarg, strerror(errno));
				return 0;
			}
			break;
		case 'o':
			if((fp_out = fopen(optarg, "w")) == NULL)
			{
				printf("%s: '%s' %s\n", argv[0], optarg, strerror(errno));
				return 0;
			}
			break;
		case 'l':
			code_list();
			return 0;
		default:
			usage(argv[0]);
			return 0;
		} // switch
	} // while

	if(tocode == NULL || fromcode == NULL) {
		usage(argv[0]);
		return (0);
	} // if

	tocode   = filter_code(tocode  );
	fromcode = filter_code(fromcode);

	if(fp_in == NULL && argc > 1 && access(argv[argc - 1], R_OK) == 0)
	{
		if((fp_in = fopen(argv[argc - 1], "r")) == NULL)
		{
			printf("%s: '%s' %s\n", argv[0], optarg, strerror(errno));
			return 0;
		}
	}

	if((conv = cconv_open(tocode, fromcode)) == (cconv_t)(-1)) {
		printf("Not support character code set.\n");
		return 0;
	} // if

#if O_BINARY
	SET_BINARY(fileno(infile));
#endif

	if(fp_in  == NULL) fp_in  = stdin ;
	if(fp_out == NULL) fp_out = stdout;
	fflush(fp_out);
	inbuf_size  = CCONV_BUFFER_SIZE;
	outbuf_size = CCONV_BUFFER_SIZE * 2;
	outlen = outbuf_size;
	i_left = CCONV_BUFFER_SIZE;

	/// read user list, main cycle.
	while(!feof(fp_in) && fgets(ps_inbuf, i_left, fp_in))
	{
		inlen = strlen(ps_inbuf);
		if(ps_inbuf[inlen - 1] != '\n')
		{
			inbuf       = (char*)realloc(inbuf , inbuf_size + CCONV_BUFFER_SIZE);
			outbuf      = (char*)realloc(outbuf, outbuf_size+ CCONV_BUFFER_SIZE * 2);
			inbuf_size += CCONV_BUFFER_SIZE;
			outbuf_size+= CCONV_BUFFER_SIZE * 2;
			outlen      = outbuf_size;
			ps_inbuf    = inbuf + strlen(inbuf);
			ps_outbuf   = outbuf;
			i_left      = inbuf_size - strlen(inbuf);
			continue;
		}

		ps_inbuf[inlen] = '\0';
		ps_inbuf = inbuf;
		inlen    = strlen(inbuf);
#ifdef FreeBSD
		ret = cconv(conv, (const char**)&ps_inbuf, &inlen, &ps_outbuf, &outlen);
#else
		ret = cconv(conv, &ps_inbuf, &inlen, &ps_outbuf, &outlen);
#endif
		if((int)ret < 0)
		{
			fprintf(stderr, "%s: [Error] %s\n", argv[0], strerror(errno));
			break;
		}

		outlen = outbuf_size;
		outbuf[ret] = '\0';
		ps_inbuf  = inbuf;
		ps_outbuf = outbuf;
		*ps_inbuf = '\0';
		i_left    = inbuf_size;
		fprintf(fp_out, "%s", outbuf);
	}

	if(*inbuf != '\0')
	{
		ps_inbuf  = inbuf;
		ps_outbuf = outbuf;
#ifdef FreeBSD
		ret = cconv(conv, (const char**)&ps_inbuf, &inlen, &ps_outbuf, &outlen);
#else
		ret = cconv(conv, &ps_inbuf, &inlen, &ps_outbuf, &outlen);
#endif
		if((int)ret < 0) {
			fprintf(stderr, "%s: [Error] %s\n", argv[0], strerror(errno));
			return 0;
		}
		fprintf(fp_out, "%s", outbuf);
	}

	fflush(fp_out);
	free(inbuf );
	free(outbuf);
	cconv_close(conv);

	if(fp_out != stdout) fclose(fp_out);
	if(fp_in  != stdin ) fclose(fp_in );
	return 0;
}

