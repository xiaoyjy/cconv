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

#ifndef _CCONV_TABLE_H_
#define _CCONV_TABLE_H_

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

#define zh_map(cd) \
	(cd == CCONV_UTF_TO_UHT) ? map_uni_s2t : map_uni_t2s

#define zh_map_size(cd) \
	(cd == CCONV_UTF_TO_UHT) ? map_uni_s2t_size : map_uni_t2s_size

#define s2t_have_cond(x) (map_uni_s2t[x].cond != -1)
#define t2s_have_cond(x) (map_uni_t2s[x].cond != -1)

#define have_cond(a, x) (a[x].cond != -1)

#define s2t_cond_ptr(x) \
	(map_uni_s2t[x].cond != -1 ? \
	&map_zh_cond[map_uni_s2t[x].cond] : NULL)

#define t2s_cond_ptr(x) \
	(map_uni_t2s[x].cond != -1 ? \
	&map_zh_cond[map_uni_t2s[x].cond] : NULL)

#define cond_ptr(a, x) \
	(a[x].cond != -1 ? \
	&map_zh_cond[a[x].cond] : NULL)

/* return char* */
#define cond_c_str(o, t) \
	(o != NULL && o->t != -1 ? zh_sets_cond[o->t] : NULL)

/* DO Make sure the cond offset is NOT -1 */
#define s2t_cond_gobal_c_str(x, t) zh_sets_cond[map_zh_cond[map_uni_s2t[x].cond].t]
#define t2s_cond_gobal_c_str(x, t) zh_sets_cond[map_zh_cond[map_uni_t2s[x].cond].t]

#define cond_gobal_c_str(a, x, t) zh_sets_cond[map_zh_cond[a[x].cond].t]

#define s2t_key(x) map_uni_s2t[x].key
#define s2t_val(x) map_uni_s2t[x].val

#define t2s_key(x) map_uni_t2s[x].key
#define t2s_val(x) map_uni_t2s[x].val

#define map_key(a, x) a[x].key
#define map_val(a, x) a[x].val

#define s2t_size() map_uni_s2t_size
#define t2s_size() map_uni_t2s_size

#endif

