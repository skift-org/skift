#include <skift/Printf.h>
#include <stdio.h>
#include <string.h>

void string_printf_append(printf_info_t *info, char c)
{
    if (info->allocated == -1)
    {
        strapd((char *)info->output, c);
    }
    else
    {
        strnapd((char *)info->output, c, info->allocated);
    }
}

int snprintf(char *s, size_t n, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vsnprintf(s, n, fmt, va);

    va_end(va);

    return result;
}

int vsnprintf(char *s, size_t n, const char *fmt, va_list va)
{
    if (n == 0)
    {
        return 0;
    }

    printf_info_t info = {};

    info.format = fmt;
    info.append = string_printf_append;
    info.output = s;
    info.allocated = n;

    // We need it to start with a 0 because we use strapd.
    s[0] = '\0';
    return __printf(&info, va);
}

int sprintf(char *s, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vsnprintf(s, 65535, fmt, va);

    va_end(va);

    return result;
}
