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

typedef struct ccchar
{
	char first;
	char second;
}
ccchar;


typedef struct condition_zh_map
{
	/* simplified to traditional, which must not match one of the word after the word */
	char* st_ma;

	/* simplified to traditional, which must not match one of the word before the word */
	char* st_mb;
	
	/* traditional to simplified, which must not match one of the word after the word */
	char* ts_ma;

	/* traditional to simplified, which must not match one of the word before the word */
	char* ts_mb;
}
condition_zh_map;

typedef struct language_zh_map
{
	char* key;
	char* val;

	/* index of the zh_map_condition array */
	short cond;
}
language_zh_map;

#include "cconv_table.cc"

#endif

