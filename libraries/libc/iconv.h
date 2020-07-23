#pragma once

#include <__libc__.h>

#include <stddef.h>

__BEGIN_HEADER

typedef void *iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode);
size_t iconv(iconv_t, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
int iconv_close(iconv_t);

__END_HEADER
