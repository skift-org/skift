#include <stdio.h>
#include <string.h>

#include <skift/io.h>

int putchar(int chr)
{
    return -1;
}

int getchar()
{
    return EOF;
}

char * gets(char * str)
{
    return 0;
}

int puts(const char * str)
{
    sk_io_print(str);
    return strlen(str);
}