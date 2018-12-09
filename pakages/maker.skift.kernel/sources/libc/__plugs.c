#include <skift/__plugs.h>

#include "kernel/serial.h"

// Plugs functions for using the skift Framework in the kernel.

void __plug_print(const char *buffer)
{
    serial_writeln((char *)buffer);
}

void __plug_putchar(char c)
{
    serial_putc(c);
}