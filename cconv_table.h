/*
 * Copyright (C) 2008, 2009
 * Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#ifndef _CCONV_TABLE_H_
#define _CCONV_TABLE_H_

#define CCONV_CODE_UTF  "UTF-8"
#define CCONV_CODE_GBK  "GBK"
#define CCONV_CODE_BIG  "BIG5"
#define CCONV_CODE_UCN  "UTF8-CN"
#define CCONV_CODE_UTW  "UTF8-TW"
#define CCONV_CODE_UHK  "UTF8-HK"

typedef enum cconv_type
{
        CCONV_ERROR = -1,
	CCONV_NULL  = 0,
        CCONV_GBK_TO_BIG = 1,
        CCONV_GBK_TO_UTT,
        CCONV_BIG_TO_GBK,
        CCONV_BIG_TO_UTS,
        CCONV_UTF_S_TO_T,
        CCONV_UTF_T_TO_S,
	CCONV_UTF_TO_GBK,
	CCONV_UTF_TO_BIG
}
cconv_type;

typedef struct factor_zh_map
{
	unsigned int n_ma;
	unsigned int n_mb;
	unsigned int y_ma;
	unsigned int y_mb;
}
factor_zh_map;

typedef struct language_zh_map
{
	char* key;
	char* val;

	/* index of the zh_map_condition array */
	unsigned int cond;
}
language_zh_map;

#include "cconv_table.cc"

#define get_zh_map(cd) \
	(cd == CCONV_UTF_S_TO_T) ? map_uni_s2t : map_uni_t2s

#define s2t_have_cond(x) (map_uni_s2t[x].cond != -1)
#define t2s_have_cond(x) (map_uni_t2s[x].cond != -1)

#define s2t_cond_ptr(x) \
	(map_uni_s2t[x].cond != -1 ? \
	&map_zh_cond[map_uni_s2t[x].cond] : NULL)

#define t2s_cond_ptr(x) \
	(map_uni_t2s[x].cond != -1 ? \
	&map_zh_cond[map_uni_t2s[x].cond] : NULL)

/* return char* */
#define get_cond_c_str(o, t) \
	(o != NULL && o->t != -1 ? zh_sets_cond[o->t] : NULL)

/* DO Make sure the cond offset is NOT -1 */
#define s2t_cond_gobal_c_str(x, t) zh_sets_cond[map_zh_cond[map_uni_s2t[x].cond].t]
#define t2s_cond_gobal_c_str(x, t) zh_sets_cond[map_zh_cond[map_uni_t2s[x].cond].t]

#define s2t_key(x) map_uni_s2t[x].key
#define s2t_val(x) map_uni_s2t[x].val

#define t2s_key(x) map_uni_t2s[x].key
#define t2s_val(x) map_uni_t2s[x].val

#define s2t_size() map_uni_s2t_size
#define t2s_size() map_uni_t2s_size

#endif

