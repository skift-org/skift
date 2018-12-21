#include <skift/__plugs.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int putchar(int chr)
{
    __plug_putchar(chr);
    return chr;
}

int getchar()
{
    return __plug_getchar();
}

char * gets(char * str)
{
    __plug_read(str, 0xFFFFFF);
    return str;
}

int puts(const char * str)
{
    __plug_print(str);
    return strlen(str);
}

int printf(const char *format, ...)
{
    va_list va;
    va_start(va, format);

    int result = vprintf(format, va);

    va_end(va);

    return result;
}

int vprintf(const char *format, va_list va)
{
    char buffer[strlen(format) + 128];
    int result = vsprintf(buffer, format, va);
    puts(buffer);

    return result;
}

int sprintf(char *str, const char *format, ...)
{
    va_list va;
    va_start(va, format);

    int result = vsprintf(str, format, va);

    va_end(va);

    return result;
}

int vsprintf(char *str, const char *format, va_list va)
{
    vsnprintf(str, 0xFFFFFF, format, va);
    return strlen(str);
}