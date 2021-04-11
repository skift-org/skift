#pragma once

#include "archs/x86/IOPort.h"
#include "archs/x86/x86.h"

#include <libsystem/Common.h>

static inline uint32_t EBP()
{
    uint32_t r;
    asm volatile("mov %%ebp, %0"
                 : "=r"(r));
    return r;
}

static inline uint32_t ESP()
{
    uint32_t r;
    asm volatile("mov %%esp, %0"
                 : "=r"(r));
    return r;
}

static inline void rdmsr(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
    asm volatile("rdmsr"
                 : "=a"(*lo), "=d"(*hi)
                 : "c"(msr));
}

static inline void wrmsr(uint32_t msr, uint32_t lo, uint32_t hi)
{
    asm volatile("wrmsr"
                 :
                 : "a"(lo), "d"(hi), "c"(msr));
}
