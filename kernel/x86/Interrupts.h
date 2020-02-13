#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef struct __packed
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
} InterruptStackFrame;

typedef uintptr_t (*IRQHandler)(uintptr_t current_stack_pointer, InterruptStackFrame *stackframe);

void interrupts_dump_stackframe(InterruptStackFrame *stackframe);

void interrupts_initialize(void);
