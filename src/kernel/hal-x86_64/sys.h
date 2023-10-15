#pragma once

#include "asm.h"
#include "gdt.h"

namespace x86_64 {

inline void sysInit(void (*handler)()) {
    wrmsr(Msrs::EFER, rdmsr(Msrs::EFER) | 1);
    wrmsr(Msrs::STAR, ((u64)(Gdt::KCODE * 8) << 32) | ((u64)(((Gdt::UDATA - 1) * 8) | 3) << 48));
    wrmsr(Msrs::LSTAR, (u64)handler);
    wrmsr(Msrs::FMASK, 0xfffffffe);
}

inline void sysSetGs(usize addr) {
    wrmsr(Msrs::UGSBAS, addr);
    wrmsr(Msrs::KGSBAS, addr);
}

} // namespace x86_64
