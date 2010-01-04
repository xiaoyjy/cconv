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

#ifndef _CCONV_UNICODE_H_
#define _CCONV_UNICODE_H_

extern int utf8_char_width (const unsigned char* w);

extern int utf8_char_encode(const unsigned char* w, int* c);
extern int utf8_char_decode(int unicode, unsigned char* utf);

#endif

