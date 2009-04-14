#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"
#include "const-c.inc"

#include <cconv.h>

MODULE = cconv		PACKAGE = cconv		

INCLUDE: const-xs.inc

SV *
cconv(const char *fromcode, const char *tocode, char *inbytes)
CODE:
	size_t inlen, outlen, size;
	cconv_t conv;
	char *out_ptr, *outbytes;
	if((conv = cconv_open(tocode, fromcode)) == (cconv_t)(-1)) {
		fprintf(stderr, "Not support character code set.\n");
	}

	inlen = strlen(inbytes);
	outlen  = 3 * inlen + 16;
	out_ptr = outbytes = (char*)malloc(outlen);
	size = cconv(conv, &inbytes, &inlen, &out_ptr, &outlen);
	cconv_close(conv);

	if(size != (size_t)(-1)) {
		RETVAL = newSVpv(outbytes, out_ptr - outbytes);
	}
	else
	{
		fprintf(stderr, "cconv: %s\n", strerror(errno));	
		RETVAL = newSVpv(NULL, 0);
	}

	OUTPUT:
		RETVAL

