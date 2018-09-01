/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include "kernel/logger.h"
#include "kernel/time.h"

void __log(const char * file, const char *message, ...)
{
    va_list va;
    va_start(va, message);
    
    printf("[%d:%d:%d] ", time_get(TIME_HOUR), time_get(TIME_MINUTE), time_get(TIME_SECOND), file);
    vprintf(message, va);
    printf(" (%s)\n", file);
    
    va_end(va);
}