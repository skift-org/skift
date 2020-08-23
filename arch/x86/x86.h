#pragma once

#include "arch/Arch.h"

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

static inline void cli() { asm volatile("cli"); }
static inline void sti() { asm volatile("sti"); }
static inline void hlt() { asm volatile("hlt"); }

static inline uint8_t in8(uint16_t port)
{
    uint8_t data;
    asm volatile("inb %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline uint16_t in16(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline uint32_t in32(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline void out8(uint16_t port, uint8_t data)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void out16(uint16_t port, uint16_t data)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void out32(uint16_t port, uint32_t data)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(data), "d"(port));
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
