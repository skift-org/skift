#pragma once

#include <libsystem/Common.h>

struct PACKED InterruptStackFrame
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
};

struct PACKED UserInterruptStackFrame
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
    uint32_t user_esp, ss;
};

void interrupts_dump_stackframe(InterruptStackFrame *stackframe);

void interrupts_initialize();
