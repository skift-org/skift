/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include "sync/atomic.h"
#include "kernel/logger.h"
#include "kernel/clock.h"

void __log(const char *file, const char *message, ...)
{
    atomic_begin();

    va_list va;
    va_start(va, message);

    printf("[%d:%d:%d] ", clock_read(T_HOUR), clock_read(T_MINUTE), clock_read(T_SECOND), file);
    vprintf(message, va);
    printf(" (%s)\n", file);

    va_end(va);

    atomic_end();
}