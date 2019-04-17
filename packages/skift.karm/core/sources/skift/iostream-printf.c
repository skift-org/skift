#include <skift/formatter.h>
#include <skift/iostream.h>

int iostream_printf(iostream_t* stream, const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = iostream_vprintf(stream, fmt, va);

    va_end(va);

    return result;
}

int iostream_printf_append();

int iostream_vprintf(iostream_t *stream, const char *fmt, va_list va)
{
    UNUSED(va);

    if (stream != NULL)
    {
        int n = 0;

        for(int i = 0; fmt[i]; i++)
        {
            iostream_write(stream, &fmt[i], 1);
            n++;
        }
        
        return n;
    }
    else
    {
        return -1;
    }
}