#pragma once

#include <libsystem/Common.h>

struct __packed InterruptStackFrame
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
};

struct __packed UserInterruptStackFrame
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
    uint32_t user_esp, ss;
};

typedef uintptr_t (*IRQHandler)(uintptr_t current_stack_pointer, InterruptStackFrame *stackframe);

void interrupts_dump_stackframe(InterruptStackFrame *stackframe);

void interrupts_initialize();
