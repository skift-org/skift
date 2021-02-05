#include <stdint.h>
#include <stdio.h>

int sprintf(char *s, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vsnprintf(s, 65535, fmt, va);

    va_end(va);

    return result;
}
