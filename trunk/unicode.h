#ifndef _CCONV_UNICODE_H_
#define _CCONV_UNICODE_H_

extern int utf_char_width(const unsigned char* w);

extern int unicode_char_encode(const unsigned char* w, int* c);

extern int unicode_char_decode(int unicode, unsigned char* utf);

#endif

