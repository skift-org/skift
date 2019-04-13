#include <skift/iostream.h>

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