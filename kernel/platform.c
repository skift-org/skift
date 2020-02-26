/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "kernel/platform.h"

size_t platform_page_size(void)
{
    return 4096;
}

/* --- FPU ------------------------------------------------------------------ */

void platform_fpu_enable(void)
{
    asm volatile("clts");
    size_t t;
    asm volatile("mov %%cr0, %0"
                 : "=r"(t));
    t &= ~(1 << 2);
    t |= (1 << 1);
    asm volatile("mov %0, %%cr0" ::"r"(t));

    asm volatile("mov %%cr4, %0"
                 : "=r"(t));
    t |= 3 << 9;
    asm volatile("mov %0, %%cr4" ::"r"(t));

    // Initialize the FPU
    asm volatile("fninit");
}

char fpu_registers[512] __aligned(16);

void platform_fpu_save_context(Task *t)
{
    asm volatile("fxsave (%0)" ::"r"(fpu_registers));
    memcpy(&t->fpu_registers, &fpu_registers, 512);
}

void platform_fpu_load_context(Task *t)
{
    memcpy(&fpu_registers, &t->fpu_registers, 512);
    asm volatile("fxrstor (%0)" ::"r"(fpu_registers));
}

/* --- Public functions ----------------------------------------------------- */

void platform_setup(void)
{
    // setup the gdt and idt

    // Setup the fpu
    logger_info("Enabling fpu...");
    platform_fpu_enable();
}

void platform_save_context(Task *task)
{
    platform_fpu_save_context(task);
}

void platform_load_context(Task *task)
{
    platform_fpu_load_context(task);
}