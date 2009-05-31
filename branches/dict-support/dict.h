/*
 * Copyright (C) 2008, 2009
 * Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#ifndef _DICT_H_
#define _DICT_H_

typedef struct {
	char* key;
	char* val;

	/* index of the zh_map_condition array */
	short cond;
} language_zh_map;
int dict_init(language_zh_map **f_dict);
void dict_unload(language_zh_map **f_dict, int f_size);
int dict_find(const char* inbytes, size_t* length, int begin, int end, int f_skip);
extern language_zh_map *g_dict;
extern int g_dict_size;

#endif

