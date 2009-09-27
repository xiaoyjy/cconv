#include <php.h>
#include <cconv.h>

#include "php_cconv.h"
#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

static function_entry cconv_functions[] = {
    PHP_FE(cconv, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry cconv_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PACKAGE_NAME,
    cconv_functions,
    NULL,                   /* module_startup_func */
    NULL,                   /* module_shutdown_func */
    NULL,                   /* request_startup_func */
    NULL,                   /* request_shutdown_func */
    NULL,                   /* info_func */
#if ZEND_MODULE_API_NO >= 20010901
    PACKAGE_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CCONV
ZEND_GET_MODULE(cconv)
#endif

PHP_FUNCTION(cconv)
{
    const char *fromcode, *tocode;
#ifdef FreeBSD
    const char *inbytes;
#else
    char *inbytes;
#endif

    cconv_t conv;
    char *outbytes, *out_ptr;
    size_t fromlen, tolen, inlen, outlen, size;

    if(ZEND_NUM_ARGS() != 3)
	RETVAL_FALSE;

    if(zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
        "sss", &fromcode, &fromlen, &tocode, &tolen, &inbytes, &inlen) == FAILURE)
    {
	RETVAL_FALSE;
    }

    if((conv = cconv_open(tocode, fromcode)) == (cconv_t)(-1)) {
	fprintf(stderr, "Not support character code set.\n");
        RETVAL_FALSE;
    }

    outlen  = 3 * inlen;
#ifdef FreeBSD
    out_ptr = outbytes = (char*)malloc(outlen);
#else
    out_ptr = outbytes = (char*)emalloc(outlen);
#endif
    size = cconv(conv, &inbytes, &inlen, &out_ptr, &outlen);
    cconv_close(conv);

    if(size == (size_t)(-1))
    {
        fprintf(stderr, "cconv: %s\n", strerror(errno));
#ifdef FreeBSD
    	free(outbytes);
#endif
        RETVAL_FALSE;
    }

    outlen = out_ptr - outbytes;
    RETVAL_STRING(outbytes, outlen);
#ifdef FreeBSD
    free(outbytes);
#else
    efree(outbytes);
#endif
}

