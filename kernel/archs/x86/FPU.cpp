#include <string.h>

#include "archs/x86/FPU.h"

char fpu_initial_context[512] ALIGNED(16);
char fpu_registers[512] ALIGNED(16);

void fpu_initialize()
{
    asm volatile("clts");
    size_t t;
    asm volatile("mov %%cr0, %0"
                 : "=r"(t));
    t &= ~(1 << 2);
    t |= (1 << 1);
    t |= (1 << 5);
    asm volatile("mov %0, %%cr0" ::"r"(t));

    asm volatile("mov %%cr4, %0"
                 : "=r"(t));
    t |= 3 << 9;
    asm volatile("mov %0, %%cr4" ::"r"(t));

    // Initialize the FPU
    asm volatile("fninit");
    asm volatile("fxsave (%0)" ::"r"(fpu_initial_context));
}

void fpu_init_context(Task *task)
{
    memcpy(&task->fpu_registers, &fpu_initial_context, 512);
}

void fpu_save_context(Task *task)
{
    asm volatile("fxsave (%0)" ::"r"(fpu_registers));
    memcpy(&task->fpu_registers, &fpu_registers, 512);
}

void fpu_load_context(Task *task)
{
    memcpy(&fpu_registers, &task->fpu_registers, 512);
    asm volatile("fxrstor (%0)" ::"r"(fpu_registers));
}
