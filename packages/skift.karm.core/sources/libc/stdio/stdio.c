/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <string.h>

#include <skift/__plugs.h>

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
