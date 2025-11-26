/*
 * Copyright (c) 2008, 2009 Yang Jianyu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

#define have_cond(a, x) (a[x].cond != -1)

#define cond_ptr(a, x) \
	(a[x].cond != -1 ? \
	&map_zh_cond[a[x].cond] : NULL)

/* return char* */
#define cond_c_str(o, t) \
	(o != NULL && o->t != -1 ? zh_sets_cond[o->t] : NULL)

/* DO Make sure the cond offset is NOT -1 */
#define cond_gobal_c_str(a, x, t) zh_sets_cond[map_zh_cond[a[x].cond].t]

#define map_key(a, x) a[x].key
#define map_val(a, x) a[x].val

#define s2t_size() map_uni_s2t_size
#define t2s_size() map_uni_t2s_size

#endif

