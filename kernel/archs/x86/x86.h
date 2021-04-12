#pragma once

#include "archs/Arch.h"

#ifdef __x86_64__
using CRRegister = uint64_t;
#else
using CRRegister = uint32_t;
#endif

static inline CRRegister CR0()
{
    CRRegister r;
    asm volatile("mov %%cr0, %0"
                 : "=r"(r));
    return r;
}

static inline CRRegister CR1()
{
    CRRegister r;
    asm volatile("mov %%cr1, %0"
                 : "=r"(r));
    return r;
}

static inline CRRegister CR2()
{
    CRRegister r;
    asm volatile("mov %%cr2, %0"
                 : "=r"(r));
    return r;
}

static inline CRRegister CR3()
{
    CRRegister r;
    asm volatile("mov %%cr3, %0"
                 : "=r"(r));
    return r;
}

static inline CRRegister CR4()
{
    CRRegister r;
    asm volatile("mov %%cr4, %0"
                 : "=r"(r));
    return r;
}

static inline void cli() { asm volatile("cli"); }

static inline void sti() { asm volatile("sti"); }

static inline void hlt() { asm volatile("hlt"); }
