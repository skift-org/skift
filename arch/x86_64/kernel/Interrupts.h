#pragma once

#include <libsystem/Common.h>

struct InterruptStackFrame
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint32_t intno, err;
};
