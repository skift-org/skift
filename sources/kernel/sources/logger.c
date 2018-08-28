/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include "kernel/logger.h"
#include "kernel/time.h"

void log(const char *message, ...)
{
    va_list va;
    va_start(va, message);
    
    printf("[%d:%d:%d] ", time_get(TIME_HOUR), time_get(TIME_MINUTE), time_get(TIME_SECOND));
    vprintf(message, va);
    putchar('\n');
    
    va_end(va);
}