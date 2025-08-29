#pragma once

// https://github.com/riscv-non-isa/riscv-sbi-doc
// https://github.com/riscv-non-isa/riscv-sbi-doc/blob/master/riscv-sbi.adoc
// https://github.com/riscv-software-src/opensbi/blob/master/docs/firmware/fw.md

import Karm.Core;

namespace Sbi {

#define FOREACH_ERROR(ERROR)     \
    ERROR(SUCCESS, 0)            \
    ERROR(FAILED, -1)            \
    ERROR(NOT_SUPPORTED, -2)     \
    ERROR(INVALID_PARAM, -3)     \
    ERROR(DENIED, -4)            \
    ERROR(INVALID_ADDRESS, -5)   \
    ERROR(ALREADY_AVAILABLE, -6) \
    ERROR(ALREADY_STARTED, -7)   \
    ERROR(ALREADY_STOPPED, -8)   \
    ERROR(NO_SHMEM, -9)

struct Ret {
    usize err;
    usize val;
};

Ret ecall(usize eid, usize fid = 0, usize arg0 = 0, usize arg1 = 0, usize arg2 = 0, usize arg3 = 0, usize arg4 = 0, usize arg5 = 0) {
    register usize a0 __asm__("a0") = arg0;
    register usize a1 __asm__("a1") = arg1;
    register usize a2 __asm__("a2") = arg2;
    register usize a3 __asm__("a3") = arg3;
    register usize a4 __asm__("a4") = arg4;
    register usize a5 __asm__("a5") = arg5;
    register usize a6 __asm__("a6") = fid;
    register usize a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                           "r"(a6), "r"(a7)
                         : "memory");

    return {a0, a1};
}

// MARK: Base Extension --------------------------------------------------------

static constexpr usize BASE = 0x10;

Ret baseGetSpecVersion() {
    return ecall(BASE, 0);
}

#define FOREACH_IMPL(ERROR)        \
    ERROR(BERKELEY_BOOT_LOADER, 0) \
    ERROR(OPENSBI, 1)              \
    ERROR(XVISOR, 2)               \
    ERROR(KVM, 3)                  \
    ERROR(RUSTSBI, 4)              \
    ERROR(DIOSIX, 5)               \
    ERROR(COFFER, 6)               \
    ERROR(XEN, 7)                  \
    ERROR(POLARFIRE_HSS, 8)

enum struct Impl : usize {
    UNKNOWN = (usize)-1,
#define ITER(NAME, VALUE) NAME = VALUE,
    FOREACH_IMPL(ITER)
#undef ITER
};

Str toStr(Impl impl) {
    switch (impl) {
#define ITER(NAME, VALUE) \
    case Impl::NAME:      \
        return #NAME;
        FOREACH_IMPL(ITER)
#undef ITER
    default:
        return "UNKNOWN";
    }
}

Ret getImplId() {
    return ecall(BASE, 1);
}

Ret getImplVersion() {
    return ecall(BASE, 2);
}

Ret probeExtension(usize ext) {
    return ecall(BASE, 3, ext);
}

Ret getMvendorid() {
    return ecall(BASE, 4);
}

Ret getMarchid() {
    return ecall(BASE, 5);
}

Ret getMimpid() {
    return ecall(BASE, 6);
}

// MARK: Legacy Extension ------------------------------------------------------

usize legacyPutChar(char c) {
    return ecall(0x1, 0, c).err;
}

usize legacyPuts(Str str) {
    for (auto c : str) {
        if (legacyPutChar(c) != 0)
            return 1;
    }
    return 0;
}

usize legacyGetChar() {
    return ecall(0x2, 0).val;
}

// MARK: System Reset Extension ------------------------------------------------

static constexpr usize SYSTEM_RESET = 0x10;

enum struct ResetType : usize {
    COLD_REBOOT = 0,
    WARM_REBOOT = 1,
    SHUTDOWN = 2,
};

enum struct ResetReason : usize {
    NONE = 0,
    SYSTEM_FAILURE = 1,
    SYSTEM_UPDATE = 2,
};

Ret systemReset(ResetType type, ResetReason reason) {
    return ecall(SYSTEM_RESET, 0, (usize)type, (usize)reason);
}

// MARK: Debug Console Extension -----------------------------------------------

static constexpr usize DEBUG_CONSOLE = 0x4442434E;

Ret debugConsoleWriteByte(char c) {
    return ecall(DEBUG_CONSOLE, 0x2, c);
}

} // namespace Sbi
