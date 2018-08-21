#include <stdio.h>
#include <stdarg.h>

#include "kernel/logging.h"
#include "kernel/time.h"

const char * const log_level[] = { "&7 ", " [warn] ", " &4[!!!] "};

void __debug(string function, string message, ...)
{
    UNUSED(function);

    va_list va;
    va_start(va, message);
    puts("&7 * &8");
    vprintf(message, va);
    puts("\n&f");
}

void __log(log_level_t level, string function, string message, ...)
{
    va_list va;
    va_start(va, message);

    printf("&3[%d:%d:%d]&7", time_get(TIME_HOUR), time_get(TIME_MINUTE), time_get(TIME_SECOND));
    printf("%s", log_level[level]); 
    printf("&e%s&f()&7 ", function);
    vprintf(message, va);
    puts("\n&f");
}

void __assert_failed(string expr, string file, string function, int line)
{
    __panic(file, function, line, NULL, "Kernel assert failed: %s !", expr);
}