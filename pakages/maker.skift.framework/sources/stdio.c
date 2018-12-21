#include <skift/__plugs.h>

#include <stdio.h>
#include <string.h>

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
