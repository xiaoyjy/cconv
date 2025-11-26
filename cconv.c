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
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "cconv.h"
#include "cconv_table.h"
#include "unicode.h"

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

typedef struct cconv_struct
{
	cconv_type cconv_cd;
	iconv_t    iconv_cd;
	iconv_t    gb_utf8;
	iconv_t    bg_utf8;
	iconv_t    utf8_gb;
	iconv_t    utf8_bg;
	int        size_factor;
	char       options[16];
}
cconv_struct;

static size_t cconv_utf8(
	const char** inbuf,
	size_t* inleft    ,
	char**  outbuf    ,
	size_t* outleft   ,
	const language_zh_map *m,
	int map_size
);

static int find_keyword(
	const char* inbytes  ,
	size_t*     length   ,
	const language_zh_map *m   ,
	int         begin    ,
	int         end      ,
	const int   whence
);

static int binary_find(
	const char* inbytes  ,
	size_t*     length   ,
	const language_zh_map *m   ,
	int         begin    ,
	int         end
);

static int match_cond(
	const factor_zh_map* cond   ,
	const char*          str    ,
	int                  klen   ,
	const int            whence
);

static int match_real_cond(
	const char* mc   ,
	const char* str  ,
	int         head ,
	const int   whence
);

/* {{{ cconv_t cconv_open(const char* tocode, const char* fromcode) */
/**
 * Open a cconv handle.
 *
 * @param   tocode	Convert to-code.
 * @param   fromcode	Convert from-code.
 * @retval  t_handle	Cconv handle,(-1: error).
 */
cconv_t cconv_open(const char* tocode, const char* fromcode)
{
	char code[8] = {0};
	char *ptr;
	cconv_struct* cd = (cconv_struct*)malloc(sizeof(cconv_struct));
    if (!cd) {  
        fprintf(stderr, "malloc failed: %s\n", strerror(errno));
        return (cconv_t)CCONV_ERROR;
    }
    memset(cd, 0, sizeof(cconv_struct));  
    cd->cconv_cd = CCONV_NULL;
    cd->size_factor = 4;

	/* //IGNORE //TRANSPORT etc. */
    if ((ptr = strstr(fromcode, "//")) != NULL) {
        size_t opt_len = ptr - fromcode;
        if (opt_len > sizeof(cd->options) - 1) {
            opt_len = sizeof(cd->options) - 1;
        }
        strncpy(cd->options, ptr, opt_len);
        cd->options[opt_len] = '\0';  // 强制null终止

        strncpy(code, fromcode, ptr - fromcode);
        code[ptr - fromcode] = '\0';  // 强制null终止
        fromcode = code;
    }

	if(0 == strcasecmp(CCONV_CODE_GBL, fromcode))
	{
		cd->gb_utf8 = iconv_open(CCONV_CODE_UTF, CCONV_CODE_GBL);
		if(0 == strcasecmp(CCONV_CODE_UHT, tocode) || 0 == strcasecmp(CCONV_CODE_UHK, tocode)
				||0 == strcasecmp(CCONV_CODE_UTW, tocode))
		{
			cd->cconv_cd = CCONV_GBL_TO_UHT;
		}
		else if(0 == strcasecmp(CCONV_CODE_UHS, tocode) || 0 == strcasecmp(CCONV_CODE_UCN, tocode))
			cd->cconv_cd = CCONV_GBL_TO_UHS;
		else if(0 == strcasecmp(CCONV_CODE_BIG, tocode))
		{
			cd->cconv_cd = CCONV_GBL_TO_BIG;
			cd->utf8_bg  = iconv_open(CCONV_CODE_BIG, CCONV_CODE_UTF);
		}
		else if(0 == strcasecmp(CCONV_CODE_GHS, tocode))
		{
			cd->cconv_cd = CCONV_GBL_TO_GHS;
			cd->utf8_gb  = iconv_open(CCONV_CODE_GBL, CCONV_CODE_UTF);
		}
		else if(0 == strcasecmp(CCONV_CODE_GHT, tocode))
		{
			cd->cconv_cd = CCONV_GBL_TO_GHT;
			cd->utf8_gb  = iconv_open(CCONV_CODE_GBL, CCONV_CODE_UTF);
		}
	}
	else
	if(0 == strcasecmp(CCONV_CODE_UTF, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UHS, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UHT, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UCN, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UHK, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UTW, fromcode)
	) {
		if(0 == strcasecmp(CCONV_CODE_UHS, tocode) || 0 == strcasecmp(CCONV_CODE_UCN, tocode))
			cd->cconv_cd = CCONV_UTF_TO_UHS;
		else if(0 == strcasecmp(CCONV_CODE_UHT, tocode) || 0 == strcasecmp(CCONV_CODE_UHK, tocode)
		     || 0 == strcasecmp(CCONV_CODE_UTW, tocode))
			cd->cconv_cd = CCONV_UTF_TO_UHT;
		else if(0 == strcasecmp(CCONV_CODE_GBL, tocode))
		{
			cd->cconv_cd = CCONV_UTF_TO_GBL;
			cd->utf8_gb  = iconv_open(CCONV_CODE_GBL, CCONV_CODE_UTF);
		}
		else if(0 == strcasecmp(CCONV_CODE_BIG, tocode))
		{
			cd->cconv_cd = CCONV_UTF_TO_BIG;
			cd->utf8_bg  = iconv_open(CCONV_CODE_BIG, CCONV_CODE_UTF);
		}

		cd->size_factor = 1;
	}
	else
	if(0 == strcasecmp(CCONV_CODE_BIG, fromcode))
	{
		if(0 == strcasecmp(CCONV_CODE_GBL, tocode))
		{
			cd->cconv_cd = CCONV_BIG_TO_GBL;
			cd->bg_utf8  = iconv_open(CCONV_CODE_UTF, CCONV_CODE_BIG);
			cd->utf8_gb  = iconv_open(CCONV_CODE_GBL, CCONV_CODE_UTF);
		}
		else if(0 == strcasecmp(CCONV_CODE_UHS, tocode) || 0 == strcasecmp(CCONV_CODE_UCN, tocode))
		{
			cd->cconv_cd = CCONV_BIG_TO_UHS;
			cd->bg_utf8  = iconv_open(CCONV_CODE_UTF, CCONV_CODE_BIG);
		}

		/* just use iconv to do others. */
	}

	if(cd->cconv_cd == CCONV_NULL)
		cd->iconv_cd = iconv_open(tocode, fromcode);
	
	if( cd->iconv_cd == (iconv_t)(-1) || cd->gb_utf8  == (iconv_t)(-1)
	 || cd->bg_utf8  == (iconv_t)(-1) || cd->utf8_gb  == (iconv_t)(-1)
	 || cd->utf8_bg  == (iconv_t)(-1)) {
        int saved_errno = errno;  
		cconv_close(cd);
        errno = saved_errno;
		return (cconv_t)(CCONV_ERROR);
	}

	return cd;
}
/* }}} */

#define cconv_iconv_first(cd) \
	ps_outbuf = ps_midbuf = (char*)malloc(o_proc); \
	if(iconv(cd, inbuf, inbytesleft, &ps_outbuf, &o_proc) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	*ps_outbuf = '\0'; \

#define cconv_cconv_second(n, o) \
	cd_struct->cconv_cd = n; \
	ps_inbuf = ps_midbuf; \
	o_proc   = strlen(ps_midbuf); \
	if((i_proc = cconv(cd, &ps_inbuf, &o_proc, outbuf, outbytesleft)) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	free(ps_midbuf); \
	cd_struct->cconv_cd = o; \
	return i_proc;

#define cconv_cconv_first(n, o) \
	ps_outbuf = ps_midbuf = (char*)malloc(o_proc); \
	cd_struct->cconv_cd = n; \
	if((i_proc = cconv(cd, inbuf, inbytesleft, &ps_outbuf, &o_proc)) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	cd_struct->cconv_cd = o; \
		
#define cconv_iconv_second(c) \
	ps_outbuf = *outbuf; \
	ps_inbuf  = ps_midbuf; \
	if(iconv(c, &ps_inbuf, &i_proc, outbuf, outbytesleft) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	free(ps_midbuf); \
	return *outbuf - ps_outbuf;

#define const_bin_c_str(x) (const unsigned char*)(x)

#define EMPTY_END_SIZE 8

/* {{{ size_t cconv() */
/**
 * Convert character code.
 *
 * @param   in_charset	Cconv input charset.
 * @param   out_charset	Cconv output charset.
 * @param   inbuf	   Input buffer.
 * @param   inbytesleft Input buffer left.
 * @retval  t_handle	Cconv handle,(-1: error).
 */
size_t cconv(cconv_t cd,
#ifdef FreeBSD
		const char** inbuf,
#else
		char** inbuf,
#endif
		size_t* inbytesleft,
		char**  outbuf,
		size_t* outbytesleft)
{

	size_t  i_proc = 0, o_proc = 0;
#ifdef FreeBSD
	const char *ps_inbuf  = NULL;
#else
	char *ps_inbuf = NULL;
#endif
	char *ps_midbuf, *ps_outbuf = NULL;
	language_zh_map *m;
	int map_size;

	if(NULL == inbuf  || NULL == *inbuf  || NULL == inbytesleft || NULL == outbuf || NULL == *outbuf || NULL == outbytesleft)
		return(size_t)(-1);

	cconv_struct *cd_struct = cd;
	ps_inbuf  = *inbuf;
	ps_outbuf = *outbuf;
	o_proc    = cd_struct->size_factor * (*inbytesleft) + EMPTY_END_SIZE;

	if((cconv_t)(CCONV_ERROR) == cd)
		return(size_t)(-1);

	switch(cd_struct->cconv_cd)
	{
	case CCONV_UTF_TO_UHT:
	case CCONV_UTF_TO_UHS:
		m        = zh_map     (cd_struct->cconv_cd);
		map_size = zh_map_size(cd_struct->cconv_cd);
		return cconv_utf8((const char**)inbuf, inbytesleft, outbuf, outbytesleft, m, map_size);

	case CCONV_UTF_TO_GBL:
		cconv_cconv_first(CCONV_UTF_TO_UHS, CCONV_UTF_TO_GBL);
		cconv_iconv_second(cd_struct->utf8_gb);

	case CCONV_UTF_TO_BIG:
		cconv_cconv_first(CCONV_UTF_TO_UHT, CCONV_UTF_TO_BIG);
		cconv_iconv_second(cd_struct->utf8_bg);

	case CCONV_GBL_TO_UHT:
		cconv_iconv_first(cd_struct->gb_utf8);
		cconv_cconv_second(CCONV_UTF_TO_UHT, CCONV_GBL_TO_UHT);

	case CCONV_GBL_TO_UHS:
		cconv_iconv_first(cd_struct->gb_utf8);
		cconv_cconv_second(CCONV_UTF_TO_UHS, CCONV_GBL_TO_UHS);

	case CCONV_GBL_TO_BIG:
		cconv_cconv_first(CCONV_GBL_TO_UHT, CCONV_GBL_TO_BIG);
		cconv_iconv_second(cd_struct->utf8_bg);

	case CCONV_GBL_TO_GHS:
		cconv_cconv_first(CCONV_GBL_TO_UHS, CCONV_GBL_TO_GHS);
		cconv_iconv_second(cd_struct->utf8_gb);

	case CCONV_GBL_TO_GHT:
		cconv_cconv_first(CCONV_GBL_TO_UHT, CCONV_GBL_TO_GHT);
		cconv_iconv_second(cd_struct->utf8_gb);

	case CCONV_BIG_TO_UHS:
		cconv_iconv_first(cd_struct->bg_utf8);
		cconv_cconv_second(CCONV_UTF_TO_UHS, CCONV_BIG_TO_UHS);

	case CCONV_BIG_TO_GBL:
		cconv_cconv_first(CCONV_BIG_TO_UHS, CCONV_BIG_TO_GBL);
		cconv_iconv_second(cd_struct->utf8_gb);
	
	case CCONV_NULL:
	default:
		break;
	} // switch

	ps_outbuf = *outbuf;
    size_t ret = iconv(cd_struct->iconv_cd, inbuf, inbytesleft, outbuf, outbytesleft);
    if (ret == (size_t)(-1)) {
        fprintf(stderr, "iconv failed: %s (errno=%d)\n", strerror(errno), errno);
        return (size_t)(-1);
    }
    return *outbuf - ps_outbuf;
}
/* }}} */

/* {{{ int cconv_close( cconv_t cd ) */
/**
 * Close a cconv handle.
 *
 * @param   cd          Cconv handle.
 * @return              0: succ, -1: fail.
 */
int cconv_close(cconv_t cd)
{
	cconv_struct *c = cd;
	if(c->iconv_cd && (iconv_t)(-1) != c->iconv_cd) iconv_close(c->iconv_cd);
	if(c->gb_utf8  && (iconv_t)(-1) != c->gb_utf8 ) iconv_close(c->gb_utf8 );
	if(c->bg_utf8  && (iconv_t)(-1) != c->bg_utf8 ) iconv_close(c->bg_utf8 );
	if(c->utf8_gb  && (iconv_t)(-1) != c->utf8_gb ) iconv_close(c->utf8_gb );
	if(c->utf8_bg  && (iconv_t)(-1) != c->utf8_bg ) iconv_close(c->utf8_bg );
	free(c);
	return 0;
}
/* }}} */

size_t cconv_utf8(const char** inbuf, size_t* inleft, char**  outbuf, size_t* outleft, const language_zh_map *m, int map_size)
{
	const char *ps_inbuf;
	char *ps_outbuf;
	int index;
	size_t i_proc, o_proc, i_conv = 0, o_conv;

	ps_inbuf  = *inbuf ;
	ps_outbuf = *outbuf;
	for (; *inleft > 0 && *outleft > 0; )
	{
		if((i_proc = utf8_char_width(const_bin_c_str(ps_inbuf))) > *inleft)
			break;

		if(i_proc > 1 &&
		  (index = find_keyword(ps_inbuf, &i_proc, m, 0, map_size - 1, i_conv)) != -1)
		{
			o_proc = strlen(map_val(m, index));
			memcpy(ps_outbuf, map_val(m, index), o_proc);
			ps_inbuf  += i_proc;
			ps_outbuf += o_proc;
			*inleft   -= i_proc;
			*outleft  -= o_proc;
			i_conv    += i_proc;
			continue;
		}
		
		if(i_proc == (size_t)(-1))
		{
			errno  = EINVAL;
            fprintf(stderr, "Invalid UTF-8 sequence at %p\n", ps_inbuf);
			return (size_t)(-2);
		}

		memcpy(ps_outbuf, ps_inbuf, i_proc);
		ps_inbuf  += i_proc;
		ps_outbuf += i_proc;
		*inleft   -= i_proc;
		*outleft  -= i_proc;
		i_conv    += i_proc;
	}

	o_conv = ps_outbuf - *outbuf;
	*ps_outbuf = '\0';
	*inbuf  = ps_inbuf;
	*outbuf = ps_outbuf;
	return o_conv;
}

int find_keyword(const char* inbytes, size_t* length, const language_zh_map *m, int begin, int end, const int whence)
{
	int location, offset;
	size_t wwidth, nwidth;

	if((offset = binary_find(inbytes, length, m, begin, end)) == -1)
		return -1;

	/* match the most accurate value */
	wwidth = *length;
	do{
		location = offset;
		*length  = wwidth;
		nwidth   = utf8_char_width(const_bin_c_str(inbytes+wwidth));
		wwidth  += nwidth;
	}
	while(nwidth != 0 && (offset = binary_find(inbytes, &wwidth, m, offset, end)) != -1);

	/* extention word fix */
	if(!match_cond(cond_ptr(m, location), inbytes, strlen(map_key(m, location)), whence))
	{
		*length = utf8_char_width(const_bin_c_str(inbytes));
		return -1;
	}

	return location;
}

/* {{{ int binary_find(cconv_t cd, const char* inbytes, int length, int begin, int end) */
int binary_find(const char* inbytes, size_t* length, const language_zh_map *m, int begin, int end)
{
	int middle, last, next_fix = 0;
	int ret, offset = -1;
	size_t width, wwidth, nwidth;

	middle = (begin + end) >> 1;
	width  = *length;
	last   = end;	
	while(1)
	{
		ret = memcmp(m[middle].key, inbytes, width);
		if(ret == 0)
		{
			if(width == strlen(m[middle].key))
				return middle;

			/* word key */
			if(next_fix == 0)
			{
				nwidth = utf8_char_width(const_bin_c_str(inbytes+width));
				wwidth = width + nwidth;
				if(nwidth != 0 && memcmp(m[middle].key, inbytes, wwidth) <= 0)
				{
					while (nwidth != 0
						&& (offset = binary_find(inbytes, &wwidth, m, offset, end)) != -1)
					{
						if(wwidth == strlen(m[offset].key))
							return offset;

						nwidth = utf8_char_width(const_bin_c_str(inbytes+width));
						wwidth += nwidth;
					}

					next_fix = 1;
				}
			}
			ret = 1;
		}

		if(ret > 0)
		{
			end = middle - 1;
			middle = (begin + end) >> 1;
		}
		else if(0 > ret)
		{
			begin = middle + 1;
			middle = (begin + end) >> 1;
		}
	
		if(end < begin) return -1;
	}

	return -1;
}
/* }}} */

int match_cond(const factor_zh_map *cond, const char* str, int klen, const int whence)
{
	int y_ma, y_mb;
	const char *cond_str = NULL;
	const char *y_a_null, *y_b_null;

	cond_str = cond_c_str(cond, n_ma);
	if(cond_str && match_real_cond(cond_str , str + klen, 0, whence))
		return 0;

	cond_str = cond_c_str(cond, n_mb);
	if(cond_str && match_real_cond(cond_str , str, 1, whence))
		return 0;

	y_b_null = cond_str = cond_c_str(cond, y_mb);
	y_ma = cond_str && match_real_cond(cond_str, str, 1, whence);
	
	y_a_null = cond_str = cond_c_str(cond, y_ma);
	y_mb = cond_str && match_real_cond(cond_str, str + klen, 0, whence);
	return (!y_b_null&&!y_a_null) | y_ma | y_mb;
}

int match_real_cond(const char* mc, const char* str, int head, const int whence)
{
	int size;
	char *m_one, *p;

	size = strlen(mc);
	p = (char *)malloc(size + 1);
	memcpy(p, mc, size);
	p[size] = '\0';

	m_one = strtok(p, ",");
	while(m_one)
	{
		if((head == 1 && whence >= strlen(m_one) &&
			memcmp(str - strlen(m_one), m_one, strlen(m_one)) == 0) 
		 ||(head == 0 && strlen(str) >= strlen(m_one) &&
			memcmp(str, m_one, strlen(m_one)) == 0)
		){
			free(p);
			return 1;
		}

		m_one = strtok(NULL, ",");
	}

	free(p);
	return 0;
}

