#ifndef _JCONV_TABLE_H_
#define _JCONV_TABLE_H_

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

