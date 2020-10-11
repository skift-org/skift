#pragma once

#include "arch/x86/kernel/IOPort.h"
#include "arch/x86/kernel/x86.h"

#include <libsystem/Common.h>

/* --- Assembly Helpers --------------------------------- */

static inline uint32_t CR0()
{
    uint32_t r;
    asm volatile("mov %%cr0, %0"
                 : "=r"(r));
    return r;
}

static inline uint32_t CR1()
{
    uint32_t r;
    asm volatile("mov %%cr1, %0"
                 : "=r"(r));
    return r;
}

static inline uint32_t CR2()
{
    uint32_t r;
    asm volatile("mov %%cr2, %0"
                 : "=r"(r));
    return r;
}

static inline uint32_t CR3()
{
    uint32_t r;
    asm volatile("mov %%cr3, %0"
                 : "=r"(r));
    return r;
}

static inline uint32_t CR4()
{
    uint32_t r;
    asm volatile("mov %%cr4, %0"
                 : "=r"(r));
    return r;
}

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
