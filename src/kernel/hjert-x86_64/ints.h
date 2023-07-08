#pragma once

#include <karm-base/array.h>

namespace Hjert::Arch {

struct [[gnu::packed]] Frame {
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rbp;
    u64 rdi;
    u64 rsi;
    u64 rdx;
    u64 rcx;
    u64 rbx;
    u64 rax;

    u64 intNo;
    u64 errNo;

    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
};

extern "C" Array<uintptr_t, 256> _intVec;

extern "C" void _intDispatch(uintptr_t rsp);

extern "C" void _sysHandler();

extern "C" uintptr_t _sysDispatch(uintptr_t rsp);

} // namespace Hjert::Arch
