#include <string.h>

size_t strlcpy(char *dst, const char *src, size_t maxlen)
{
    const size_t srclen = strlen(src);

    if (srclen + 1 < maxlen)
    {
        memcpy(dst, src, srclen + 1);
    }
    else if (maxlen != 0)
    {
        memcpy(dst, src, maxlen - 1);
        dst[maxlen - 1] = '\0';
    }

    return srclen;
}

void strrvs(char *str)
{
    int c, i, j;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}
