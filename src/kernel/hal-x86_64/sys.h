#pragma once

#include "asm.h"
#include "gdt.h"

namespace x86_64 {

inline void sysInit(void (*handler)()) {
    wrmsr(Msrs::EFER, rdmsr(Msrs::EFER) | 1);
    wrmsr(Msrs::STAR, ((uint64_t)(Gdt::KCODE * 8) << 32) | ((uint64_t)(((Gdt::UCODE - 1) * 8) | 3) << 48));
    wrmsr(Msrs::LSTAR, (uint64_t)handler);
    wrmsr(Msrs::SYSCALL_FLAG_MASK, 0xfffffffe);
}

inline void sysSetGs(uintptr_t addr) {
    wrmsr(Msrs::GS_BASE, addr);
    wrmsr(Msrs::KERN_GS_BASE, addr);
}

} // namespace x86_64
