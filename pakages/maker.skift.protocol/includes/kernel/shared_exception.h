#pragma once
#include <skift/generic.h>

typedef enum
{
    EXCEP_DEBUG,
    EXCEP_BREAKPOINT,
    EXCEP_DIVIDE_BY_ZERO,

    EXCEP_STACK_OVERFLOW,
    EXCEP_SEGFAULT,
} exception_t;