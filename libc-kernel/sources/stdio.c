#ifdef __KERNEL

#include <stdio.h>
#include <string.h>

#include "kernel/console.h"

int putchar(int chr)
{
    console_put(chr);
    return chr;
}

int getchar()
{
    return EOF;
}

char * gets(char * str)
{
   UNUSED(str);
   return NULL;
}

int puts(const char * str)
{
    console_puts((char *)str);
    return strlen(str);
}

#endif