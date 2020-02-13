#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

static inline u32 CR0()
{
    u32 r;
    asm volatile("mov %%cr0, %0"
                 : "=r"(r));
    return r;
}

static inline u32 CR1()
{
    u32 r;
    asm volatile("mov %%cr1, %0"
                 : "=r"(r));
    return r;
}

static inline u32 CR2()
{
    u32 r;
    asm volatile("mov %%cr2, %0"
                 : "=r"(r));
    return r;
}

static inline u32 CR3()
{
    u32 r;
    asm volatile("mov %%cr3, %0"
                 : "=r"(r));
    return r;
}

static inline u32 CR4()
{
    u32 r;
    asm volatile("mov %%cr4, %0"
                 : "=r"(r));
    return r;
}

static inline void cli(void) { asm volatile("cli"); }
static inline void sti(void) { asm volatile("sti"); }
static inline void hlt(void) { asm volatile("hlt"); }

static inline u8 in8(u16 port)
{
    u8 data;
    asm volatile("inb %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline u16 in16(u16 port)
{
    u16 data;
    asm volatile("inw %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline u32 in32(u16 port)
{
    u32 data;
    asm volatile("inl %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline void out8(u16 port, u8 data)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(data), "d"(port));
}

static inline void out16(u16 port, u16 data)
{
    asm volatile("outw %0, %1" ::"a"(data), "d"(port));
}

static inline void out32(u16 port, u32 data)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(data), "d"(port));
}