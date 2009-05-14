#include "unicode.h"

int utf_char_width(const unsigned char* w)
{
	if(w[0] < 0x80) return 1;
	if(w[0] < 0xe0) return 2;
	if(w[0] < 0xf0) return 3;
	if(w[0] < 0xf8) return 4;
	if(w[0] < 0xfc) return 5;
	if(w[0] < 0xfe) return 6;

	return -1;
}

int unicode_char_encode(const unsigned char* w, int* c)
{
	/* 00000000-01111111	00-7F	0-127	US-ASCII (single byte) */
	if(w[0] < 0x80) {
		*c = (int)w[0];
		return 1;
	}

	/* 10000000-10111111	80-BF	128-191	Second, third, or fourth byte of a multi-byte sequence */
	/* 11000000-11000001	C0-C1	192-193	Overlong encoding: start of a 2-byte sequence, but code point <= 127*/

	/* 11000010-11011111	C2-DF	194-223	Start of 2-byte sequence */
	if(w[0] < 0xe0 && w[0] > 0xc1) {
		*c = (int)(((w[0] - 0xc0) << 6) + w[1] - 0x80);
		return 2;
	}

	/* 11100000-11101111	E0-EF	224-239	Start of 3-byte sequence */
	if(w[0] < 0xf0 && w[0] > 0xdf)
	{
		*c = (int)(((w[0] - 0xe0) << 12) + ((w[1] - 0x80) << 6) + w[2] - 0x80);
		return 3;
	}

	/* 11110000-11110100	F0-F4	240-244	Start of 4-byte sequence */
	if(w[0] < 0xf8 && w[0] > 0xef)
	{
		*c = (int)(
			((w[0] - 0xf0) << 18) +
			((w[1] - 0x80) << 12) +
			((w[2] - 0x80) << 6 ) + w[3] - 0x80);
		return 4;
	}
	/* 11110101-11110111	F5-F7	245-247	Restricted by RFC 3629:
	 * start of 4-byte sequence for codepoint above 10FFFF
	 */


	/* 11111000-11111011	F8-FB	248-251	Restricted by RFC 3629: start of 5-byte sequence */
	if(w[0] < 0xfc && w[0] > 0xf7)
	{
		*c = (int)(
			((w[0] - 0xf8) << 24) +
			((w[1] - 0x80) << 18) +
			((w[2] - 0x80) << 12) +
			((w[3] - 0x80) << 6 ) + w[4] - 0x80);
		return 5;
	}

	/* 11111100-11111101	FC-FD	252-253	Restricted by RFC 3629: start of 6-byte sequence */
	if(w[0] < 0xfe && w[0] > 0xfb)
	{
		*c = (int)(
			((w[0] - 0xfc) << 30) +
			((w[1] - 0x80) << 24) +
			((w[2] - 0x80) << 18) +
			((w[3] - 0x80) << 12) +
			((w[4] - 0x80) << 6 ) + w[5] - 0x80);
		return 6;
	}

	*c = 0;
	return -1;
}

int unicode_char_decode(int unicode, unsigned char* utf)
{
	int count = -1;

	     if(unicode < 0x80)	       /*0100,0000*/
		count = 1;
	else if (unicode < 0x800)      /*0100,0000,0000*/
		count = 2;
	else if (unicode < 0x10000)    /*0001,0000,0000,0000,000,*/
		count = 3;
	else if (unicode < 0x200000)   /*0010,0000,0000,0000,0000,0000*/
		count = 4;
	else if (unicode < 0x4000000)  /*0100,0000,0000,0000,0000,0000,0000*/
		count = 5;
	else if (unicode <= 0x7fffffff)/*0110,1111,1111,1111,1111,1111,1111,1111*/
		count = 6;
	else
		return count;

	switch (count) { /* note: code falls through cases! */
		case 6: utf[5] = 0x80 | (unicode & 0x3f); unicode = unicode >> 6; unicode |= 0x4000000;
		case 5: utf[4] = 0x80 | (unicode & 0x3f); unicode = unicode >> 6; unicode |= 0x200000;
		case 4: utf[3] = 0x80 | (unicode & 0x3f); unicode = unicode >> 6; unicode |= 0x10000;
		case 3: utf[2] = 0x80 | (unicode & 0x3f); unicode = unicode >> 6; unicode |= 0x800;
		case 2: utf[1] = 0x80 | (unicode & 0x3f); unicode = unicode >> 6; unicode |= 0xc0;
		case 1: utf[0] = (char)unicode;
	}

	return count;	
}


