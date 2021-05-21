#pragma once

#include "archs/x86/x86.h"

namespace Arch::x86_64
{

static inline uint64_t RBP()
{
    uint64_t r;
    asm volatile("mov %%rbp, %0"
                 : "=r"(r));
    return r;
}

} // namespace Arch::x86_64
