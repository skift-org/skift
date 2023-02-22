#pragma once

#include <karm-base/array.h>

namespace Hjert::Arch {

struct [[gnu::packed]] Frame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t intNo;
    uint64_t errNo;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

extern "C" Array<uintptr_t, 256> _intVec;

extern "C" uintptr_t _intDispatch(uintptr_t rsp);

extern "C" void _sysHandler();

extern "C" uintptr_t _sysDispatch(uintptr_t rsp);

} // namespace Hjert::Arch
