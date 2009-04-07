/*
 * Copyright (C) 2008, 2009
 * Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#ifndef _CCONV_H_
#define _CCONV_H_

#include <iconv.h>

typedef void* cconv_t;

/* {{{ cconv_t cconv_open(const char* tocode, const char* fromcode) */
/**
 * Open a cconv handle.
 *
 * @param   tocode	Convert to-code.
 * @param   fromcode	Convert from-code.
 * @retval  t_handle	Cconv handle,(-1: error).
 */
extern cconv_t cconv_open(const char* tocode, const char* fromcode);


/**
 * Convert character code.
 *
 * @param   t_handle    cconv handle.
 * @param   inbuf       Input buffer.
 * @param   inbytesleft Input buffer left.
 * @retval  t_handle    cconv handle,(-1: error).
 */
extern size_t cconv(cconv_t cd,
#ifdef FreeBSD
		const char** inbuf,
#else
		char** inbuf,
#endif
		size_t* inbytesleft,
		char**  outbuf,
		size_t* outbytesleft);


/* {{{ int cconv_close( cconv_t cd ) */
/**
 * Close a cconv handle.
 *
 * @param   cd          cconv handle.
 * @return              0: succ, -1: fail.
 */
extern int cconv_close(cconv_t cd);

#endif

