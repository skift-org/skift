#include <stdarg.h>
#include <stdlib.h>

#include <skift/__plugs.h>

#include <skift/console.h>
#include <skift/string.h>

void sk_console_print(const char *msg)
{
    __plug_print(msg);
}

void sk_console_println(const char *msg)
{
    __plug_print(msg);
    __plug_putchar('\n');
}

/* --- Printf logic --------------------------------------------------------- */
/*void sk_console_printf(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    
    va_end(va);
}*/