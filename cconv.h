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

#ifndef _CCONV_H_
#define _CCONV_H_

#include <iconv.h>

#define CCONV_CODE_UTF  "UTF-8"

// GBL means LARGEST of GB encoding.
#define CCONV_CODE_GBL  "GB18030"
#define CCONV_CODE_BIG  "BIG5"

#define CCONV_CODE_UCN  "UTF8-CN"
#define CCONV_CODE_UTW  "UTF8-TW"
#define CCONV_CODE_UHK  "UTF8-HK"

#define CCONV_CODE_UHS  "UTF8-HANS"
#define CCONV_CODE_UHT  "UTF8-HANT"

#define CCONV_CODE_GHS  "GB-HANS"
#define CCONV_CODE_GHT  "GB-HANT"

typedef enum cconv_type
{
	CCONV_ERROR = -1,
	CCONV_NULL  = 0 ,
	CCONV_GBL_TO_BIG = 1,
	CCONV_GBL_TO_UHS,
	CCONV_GBL_TO_UHT,
	CCONV_GBL_TO_GHS,
	CCONV_GBL_TO_GHT,
	CCONV_BIG_TO_GBL,
	CCONV_BIG_TO_UHS,
	CCONV_UTF_TO_UHT,
	CCONV_UTF_TO_UHS,
	CCONV_UTF_TO_GBL,
	CCONV_UTF_TO_BIG,
}
cconv_type;

typedef void* cconv_t;

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _CCONV_H_

