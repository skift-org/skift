#include <stdio.h>
#include <string.h>

#include "kernel/serial.h"

int putchar(int chr)
{
    serial_putc(chr);
    return chr;
}

int getchar()
{
    return serial_getc();
}

char * gets(char * str)
{
   serial_read(str, 100);
   return str;
}

int puts(const char * str)
{
    serial_writeln((char *)str);
    return strlen(str);
}