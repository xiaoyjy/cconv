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
}
cconv_struct;

static int find_keyword(
	cconv_type  cd       ,
	const char* inbytes  ,
	size_t*     length   ,
	int         begin    ,
	int         end      ,
	const int   whence
);

static int binary_find(
	cconv_type  cd       ,
	const char* inbytes  ,
	size_t*     length   ,
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
	cconv_struct* cd = (cconv_struct*)malloc(sizeof(cconv_struct));
	cd->cconv_cd = CCONV_NULL;
	cd->iconv_cd = NULL;

	if(0 == strcasecmp(CCONV_CODE_GBK, fromcode)) {

		if(0 == strcasecmp(CCONV_CODE_BIG, tocode))
			cd->cconv_cd = CCONV_GBK_TO_BIG;
		else if(0 == strcasecmp(CCONV_CODE_UHK, tocode)
		     || 0 == strcasecmp(CCONV_CODE_UTW, tocode)
		)
			cd->cconv_cd = CCONV_GBK_TO_UTT;
	}
	else
	if(0 == strcasecmp(CCONV_CODE_BIG, fromcode)) {

		if(0 == strcasecmp(CCONV_CODE_GBK, tocode))
			cd->cconv_cd = CCONV_BIG_TO_GBK;

		else if(0 == strcasecmp(CCONV_CODE_UCN, tocode))
			cd->cconv_cd = CCONV_BIG_TO_UTS;
	}
	else
	if(0 == strcasecmp(CCONV_CODE_UTF, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UCN, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UHK, fromcode)
	 ||0 == strcasecmp(CCONV_CODE_UTW, fromcode)
	) {
		if(0 == strcasecmp(CCONV_CODE_UCN, tocode))
			cd->cconv_cd = CCONV_UTF_T_TO_S;
		else if(0 == strcasecmp(CCONV_CODE_UHK, tocode) ||
				 0 == strcasecmp(CCONV_CODE_UTW, tocode))
			cd->cconv_cd = CCONV_UTF_S_TO_T;
		else if(0 == strcasecmp(CCONV_CODE_GBK, tocode))
			cd->cconv_cd = CCONV_UTF_TO_GBK;
		else if(0 == strcasecmp(CCONV_CODE_BIG, tocode))
			cd->cconv_cd = CCONV_UTF_TO_BIG;
	}

	if(cd->cconv_cd == CCONV_ERROR)
		return (cconv_t)(-1);

	if(cd->cconv_cd != CCONV_NULL)
		return cd;

	cd->iconv_cd = iconv_open(tocode, fromcode);
	if(cd->iconv_cd == (iconv_t)(-1))
		return (cconv_t)(-1);

	return cd;
}
/* }}} */

#define cconv_iconv_first(to, from) \
	ps_outbuf = ps_midbuf = (char*)malloc(o_proc); \
	cd_struct->iconv_cd = iconv_open(to, from); \
	if(cd_struct->iconv_cd == (iconv_t)(-1)) { \
		errno = EILSEQ; \
		free(ps_midbuf); \
		return (size_t)(-1); \
	} \
	  \
	if(iconv(cd_struct->iconv_cd, inbuf, inbytesleft, &ps_outbuf, &o_proc) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	  \
	iconv_close(cd_struct->iconv_cd); cd_struct->iconv_cd = NULL; \
	*ps_outbuf = '\0';

#define cconv_cconv_second(type) \
	cd_struct->cconv_cd = type; \
	ps_inbuf = ps_midbuf; \
	o_proc = strlen(ps_inbuf); \
	if((i_conv = cconv(cd_struct, &ps_inbuf, &o_proc, outbuf, outbytesleft)) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	free(ps_midbuf);

#define cconv_cconv_first(type) \
	ps_outbuf = ps_midbuf = (char*)malloc(o_proc); \
	cd_struct->cconv_cd = type; \
	if(cconv(cd_struct, inbuf, inbytesleft, &ps_outbuf, &o_proc) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	}

#define cconv_iconv_second(to, from) \
	cd_struct->iconv_cd = iconv_open(to, from); \
	if(cd_struct->iconv_cd == (iconv_t)(-1)) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	  \
	ps_outbuf = *outbuf; \
	ps_inbuf  = ps_midbuf; \
	o_proc = strlen(ps_inbuf); \
	if(iconv(cd_struct->iconv_cd, &ps_inbuf, &o_proc, outbuf, outbytesleft) == -1) { \
		free(ps_midbuf); return (size_t)(-1); \
	} \
	  \
	iconv_close(cd_struct->iconv_cd); \
	cd_struct->iconv_cd = NULL; \
	free(ps_midbuf);

#define const_bin_c_str(x) (const unsigned char*)(x)

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
	size_t  i_conv	= 0;
	size_t  i_proc	= 0;
	size_t  o_proc  = 0;
	int     i_offset  = 0,
		index = 0;
#ifdef FreeBSD
	const char *ps_inbuf  = NULL;
#else
	char *ps_inbuf = NULL;
#endif
	char *ps_outbuf = NULL;
	char *ps_midbuf;

	cconv_struct *cd_struct = cd;
	ps_inbuf = *inbuf;
	ps_outbuf = *outbuf;

	if(NULL == inbuf  || NULL == *inbuf  || NULL == inbytesleft
	   		|| NULL == outbuf || NULL == *outbuf || NULL == outbytesleft)
		return(size_t)(-1);

	switch(cd_struct->cconv_cd)
	{
	case CCONV_GBK_TO_UTT:
		o_proc = 4 * (*inbytesleft) + 16;
		cconv_iconv_first (CCONV_CODE_UTF, CCONV_CODE_GBK);
		cconv_cconv_second(CCONV_UTF_S_TO_T);
		cd_struct->cconv_cd = CCONV_GBK_TO_UTT;
		return i_conv;

	case CCONV_BIG_TO_UTS:
		o_proc = 4 * (*inbytesleft) + 16;
		cconv_iconv_first (CCONV_CODE_UTF, CCONV_CODE_BIG);
		cconv_cconv_second(CCONV_UTF_T_TO_S);
		cd_struct->cconv_cd = CCONV_BIG_TO_UTS;
		return i_conv;

	case CCONV_GBK_TO_BIG:
		o_proc = 4 * (*inbytesleft) + 128;
		cconv_cconv_first (CCONV_GBK_TO_UTT);
		cconv_iconv_second(CCONV_CODE_BIG, CCONV_CODE_UTF);
		cd_struct->cconv_cd = CCONV_GBK_TO_BIG;
		return *outbuf - ps_outbuf;

	case CCONV_BIG_TO_GBK:
		o_proc = 4 * (*inbytesleft) + 128;
		cconv_cconv_first (CCONV_BIG_TO_UTS);
		cconv_iconv_second(CCONV_CODE_GBK, CCONV_CODE_UTF);
		cd_struct->cconv_cd = CCONV_BIG_TO_GBK;
		return *outbuf - ps_outbuf;

	case CCONV_UTF_TO_GBK:
		o_proc = *inbytesleft + 512;
		cconv_cconv_first (CCONV_UTF_T_TO_S);
		cconv_iconv_second(CCONV_CODE_GBK, CCONV_CODE_UTF);
		cd_struct->cconv_cd = CCONV_UTF_TO_GBK;
		return *outbuf - ps_outbuf;

	case CCONV_UTF_TO_BIG:
		o_proc = *inbytesleft + 512;
		cconv_cconv_first (CCONV_UTF_S_TO_T);
		cconv_iconv_second(CCONV_CODE_BIG, CCONV_CODE_UTF);
		cd_struct->cconv_cd = CCONV_UTF_TO_BIG;
		return *outbuf - ps_outbuf;
	
	case CCONV_UTF_S_TO_T:
	for (; (*inbytesleft) > 0 && (*outbytesleft) > 0; ) {
		i_proc = utf_char_width(const_bin_c_str(ps_inbuf));
		if(i_proc > (*inbytesleft))
			break;

		if(i_proc > 1)
		{
			index = find_keyword(CCONV_UTF_S_TO_T, ps_inbuf, &i_proc, 0, s2t_size() - 1, i_offset);
			if(index != -1)
			{
				o_proc = strlen(s2t_val(index));
				memcpy(ps_outbuf + i_conv, s2t_val(index), o_proc);
				ps_inbuf	+= i_proc;
				(*inbytesleft)  -= i_proc;
				(*outbytesleft) += o_proc;
				i_conv          += o_proc;
				i_offset         = (*inbuf) - ps_inbuf;
				continue;
			}
		}
		else if(i_proc == -1)
		{
			errno  = EINVAL;
			return (size_t)(-2);
		}

		memcpy(ps_outbuf + i_conv, ps_inbuf, i_proc);
		ps_inbuf	+= i_proc;
		(*inbytesleft)  -= i_proc;
		(*outbytesleft) += i_proc;
		i_conv	        += i_proc;
		i_offset         = (*inbuf) - ps_inbuf;
	} // for

	*inbuf  = ps_inbuf;
	*outbuf = ps_outbuf + i_conv;
	return i_conv;

	case CCONV_UTF_T_TO_S:
	for (; (*inbytesleft) > 0 && (*outbytesleft) > 0; ) {
		i_proc = utf_char_width(const_bin_c_str(ps_inbuf));
		if(i_proc > (*inbytesleft))
			break;

		if(i_proc > 1)
		{
			index = find_keyword(CCONV_UTF_T_TO_S, ps_inbuf, &i_proc, 0, t2s_size() - 1, i_offset);
			if(index != -1)
			{
				o_proc = strlen(t2s_val(index));
				memcpy(ps_outbuf + i_conv, t2s_val(index), o_proc);
				ps_inbuf	+= i_proc;
				(*inbytesleft)  -= i_proc;
				(*outbytesleft) += o_proc;
				i_conv          += o_proc;
				i_offset         = (*inbuf) - ps_inbuf;
				continue;
			}
		}
		else if(i_proc == -1)
		{
			errno  = EINVAL;
			return (size_t)(-2);
		}

		memcpy(ps_outbuf + i_conv, ps_inbuf, i_proc);
		ps_inbuf	+= i_proc;
		(*inbytesleft)  -= i_proc;
		(*outbytesleft) += i_proc;
		i_conv          += i_proc;
		i_offset         = (*inbuf) - ps_inbuf;
	} // for
	*inbuf  = ps_inbuf;
	*outbuf = ps_outbuf + i_conv;
	return i_conv;

	case CCONV_NULL:
	default:
		break;
	} // switch

	ps_outbuf = *outbuf;
	if(iconv(cd_struct->iconv_cd, inbuf, inbytesleft, outbuf, outbytesleft) == -1)
		return (size_t)(-1);

	return *outbuf - ps_outbuf;
}
/* }}} */


/* {{{ int cconv_close( cconv_t cd ) */
/**
 * Close a cconv handle.
 *
 * @param   cd          Jconv handle.
 * @return              0: succ, -1: fail.
 */
int cconv_close( cconv_t cd )
{
	cconv_struct *cd_struct = cd;
	iconv_t icd = cd_struct->iconv_cd;
	if(cd != NULL && icd != NULL && icd != (iconv_t)(-1))
		iconv_close(cd_struct->iconv_cd);

	free(cd);
	cd = NULL;
	return(0);
}
/* }}} */


/* {{{ int binary_find(cconv_t cd, const char* inbytes, int length, int begin, int end) */
int binary_find(cconv_type cd, const char* inbytes, size_t* length, int begin, int end)
{
	language_zh_map *map;
	int middle, last, next_fix = 0;
	int ret, offset = -1;
	size_t width, wwidth, nwidth;

	map = get_zh_map(cd);
	middle = (begin + end) >> 1;
	width  = *length;
	last   = end;
	
	while(1)
	{
		ret = memcmp(map[middle].key, inbytes, width);
		if(ret == 0)
		{
			if(width == strlen(map[middle].key))
				return middle;

			/* word key */
			if(next_fix == 0)
			{
				nwidth = utf_char_width(const_bin_c_str(inbytes+width));
				wwidth = width + nwidth;
				if(nwidth != 0 && memcmp(map[middle].key, inbytes, wwidth) <= 0)
				{
					while (nwidth != 0
						&& (offset = binary_find(cd, inbytes, &wwidth, offset, end)) != -1)
					{
						if(wwidth == strlen(map[offset].key))
							return offset;

						nwidth = utf_char_width(const_bin_c_str(inbytes+width));
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

int find_keyword(cconv_type cd, const char* inbytes, size_t* length, int begin, int end, const int whence)
{
	int location, offset;
	size_t wwidth, nwidth;

	if((offset = binary_find(cd, inbytes, length, begin, end)) == -1)
		return -1;

	/* match the most accurate value */
	wwidth = *length;
	do{
		location = offset;
		*length  = wwidth;
		nwidth   = utf_char_width(const_bin_c_str(inbytes+wwidth));
		wwidth  += nwidth;
	}
	while(nwidth != 0 && (offset = binary_find(cd, inbytes, &wwidth, offset, end)) != -1);

	/* extention word fix, s to t */
	if(cd == CCONV_UTF_S_TO_T)
	{
		if(!match_cond(s2t_cond_ptr(location), inbytes, strlen(s2t_key(location)), whence))
		{
			*length = utf_char_width(const_bin_c_str(inbytes));
			return -1;
		}
	}

	/* extention word fix, t to s */
	if(cd == CCONV_UTF_T_TO_S)
	{
		if(!match_cond(t2s_cond_ptr(location), inbytes, strlen(t2s_key(location)), whence))
		{
			*length = utf_char_width(const_bin_c_str(inbytes));
			return -1;
		}
	}

	return location;
}

int match_cond(const factor_zh_map *cond, const char* str, int klen, const int whence)
{
	int y_ma, y_mb;
	const char *cond_str = NULL;
	const char *y_a_null, *y_b_null;

	cond_str = get_cond_c_str(cond, n_ma);
	if(cond_str && match_real_cond(cond_str , str + klen, 0, whence))
		return 0;

	cond_str = get_cond_c_str(cond, n_mb);
	if(cond_str && match_real_cond(cond_str , str, 1, whence))
		return 0;

	y_b_null = cond_str = get_cond_c_str(cond, y_mb);
	y_ma = cond_str && match_real_cond(cond_str, str, 1, whence);
	
	y_a_null = cond_str = get_cond_c_str(cond, y_ma);
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

