#pragma once

#include <libsystem/Logger.h>

#include "arch/Arch.h"
#include "arch/x86/Interrupts.h"

#define HANG         \
    while (1)        \
    {                \
        arch_halt(); \
    }

#define STOP                      \
    while (1)                     \
    {                             \
        arch_disable_interupts(); \
        arch_halt();              \
    }

void __no_return __panic(const char *file, const char *function, const int line, InterruptStackFrame *stackframe, const char *message, ...);

#define PANIC(x...) __panic(__FILE__, __FUNCTION__, __LINE__, NULL, x)

#define CPANIC(ctx, x...) __panic(__FILE__, __FUNCTION__, __LINE__, ctx, x)
