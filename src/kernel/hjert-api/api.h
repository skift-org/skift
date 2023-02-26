#pragma once

#include <hal/vmm.h>
#include <karm-base/array.h>
#include <karm-base/enum.h>
#include <karm-base/macros.h>
#include <karm-base/res.h>

namespace Hj {

/* --- Basic Data Types ----------------------------------------------------- */

enum struct Type {
    NONE,

    NODE,
    TASK,
    SPACE,
    MEM,
    IO,
};

enum struct Syscall {
    NONE,

    LOG,

    CREATE,
    DROP,
    DUP,

    START,
    WAIT,
    RET,

    MAP,
    UNMAP,

    IN,
    OUT,

    IPC,

    _LEN,
};

enum struct [[nodiscard]] Code {
    _OK,

    BAD_SYSCALL,
    BAD_CAP,
    BAD_TYPE,
    BAD_ADDR,
    NOT_IMPLEMENTED,

    _LEN,
};

template <typename T = None>
struct [[nodiscard]] Res : public Karm::Res<T, Hj::Code> {
    using Karm::Res<T, Hj::Code>::Res;
};

using Arg = uintptr_t;

using Args = Array<Arg, 6>;

enum struct MsgFlags {
    NONE = 0,

    CAP0 = 1 << 0,
    CAP1 = 1 << 1,
    CAP2 = 1 << 2,
    CAP3 = 1 << 3,
    CAP4 = 1 << 4,
    CAP5 = 1 << 5,
};

struct Msg {
    Arg label;
    MsgFlags flags{};
    Args args{};

    constexpr Msg(Arg label)
        : label(label) {}
};

struct Cap {
    Arg _raw;

    constexpr Arg raw() const { return _raw; }
};

inline constexpr Cap SELF = Cap{0};

/* --- Syscall Interface ---------------------------------------------------- */

// clang-format off

#define SYSCALL(name) [[gnu::always_inline, gnu::used]] static inline Res<> name

SYSCALL(syscall) (Syscall syscall, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0);

#ifdef __osdk_arch_x86_64__

SYSCALL(syscall) (Syscall s, Arg a0, Arg a1, Arg a2, Arg a3, Arg a4, Arg a5) {
    Code c;

    // s : rax, arg1 : rdi, arg2 : rsi, arg3 : rdx, arg4 : r10, arg5 : r8, arg6 : r9
    __asm__ volatile(
        "syscall"
        : "=a"(c)
        : "a"(s), "D"(a0), "S"(a1), "d"(a2), "r"(a3), "r"(a4), "r"(a5)
        : "rcx", "r11", "memory");

    if (c != Code::_OK)
        return c;

    return Ok();
}

#else
#    error "Unsupported architecture"
#endif

SYSCALL(log) (char const *msg, size_t len) {
    return syscall(Syscall::LOG, (Arg)msg, len);
}

SYSCALL(create) (Type type, Cap *cap, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0) {
    return syscall(Syscall::CREATE, (Arg)type, (Arg)cap, a0, a1, a2, a3);
}

SYSCALL(createNode) (Cap* cap, size_t len = 512) {
    return create(Type::NODE, cap, len);
}

SYSCALL(createTask) (Cap* cap, Cap node, Cap space) {
    return create(Type::TASK, cap, node.raw(), space.raw());
}

SYSCALL(createSpace) (Cap* cap) {
    return create(Type::SPACE, cap);
}

enum struct MemFlags : Arg {
    NONE = 0,
    LOW = 1 << 0,
    HIGH = 1 << 1,
    DMA = 1 << 2,
};

FlagsEnum$(MemFlags);

SYSCALL(createMem) (Cap* cap, uintptr_t phys, size_t len, MemFlags flags = MemFlags::NONE) {
    return create(Type::MEM, cap, phys, len, (Arg)flags);
}


SYSCALL(createMem) (Cap* cap, size_t len, MemFlags flags = MemFlags::NONE) {
    return create(Type::MEM, cap, -1, len, (Arg)flags);
}

SYSCALL(createIo) (Cap* cap, uintptr_t base, size_t len) {
    return create(Type::IO, cap, base, len);
}

SYSCALL(drop) (Cap cap) {
    return syscall(Syscall::DROP, cap.raw());
}

SYSCALL(dup) (Cap node, Cap *dst, Cap src) {
    return syscall(Syscall::DUP, node.raw(), (Arg)dst, src.raw());
}

SYSCALL(start) (Cap cap,uintptr_t ip, uintptr_t sp, Args const* args) {
    return syscall(Syscall::START, cap.raw(), ip, sp, (Arg)args);
}

SYSCALL(wait) (Cap cap, Arg *ret) {
    return syscall(Syscall::WAIT, cap.raw(), (Arg)ret);
}

SYSCALL(ret) (Cap cap, Arg ret) {
    return syscall(Syscall::RET, cap.raw(), ret);
}

using MapFlags = Hal::VmmFlags;

SYSCALL(map) (Cap cap, uintptr_t* virt, Cap mem, uintptr_t off, size_t len, MapFlags flags = MapFlags::NONE) {
    return syscall(Syscall::MAP, cap.raw(), (Arg)virt, mem.raw(), off, len, (Arg)flags);
}

SYSCALL(unmap) (Cap cap, uintptr_t virt, size_t len) {
    return syscall(Syscall::UNMAP, cap.raw(), virt, len);
}

enum struct IoLen : Arg {
    U8 ,
    U16,
    U32,
    U64,
};

SYSCALL(in) (Cap cap, IoLen len, uintptr_t port, Arg *val) {
    return syscall(Syscall::IN, cap.raw(), (Arg)len, port, (Arg)val);
}

SYSCALL(in8) (Cap cap, uintptr_t port, uint8_t *val) {
    return syscall(Syscall::IN, cap.raw(), (Arg)IoLen::U8, port, (Arg)val);
}

SYSCALL(in16) (Cap cap, uintptr_t port, uint16_t *val) {
    return syscall(Syscall::IN, cap.raw(), (Arg)IoLen::U16, port, (Arg)val);
}

SYSCALL(in32) (Cap cap, uintptr_t port, uint32_t *val) {
    return syscall(Syscall::IN, cap.raw(), (Arg)IoLen::U32, port, (Arg)val);
}

SYSCALL(in64) (Cap cap, uintptr_t port, uint64_t *val) {
    return syscall(Syscall::IN, cap.raw(), (Arg)IoLen::U64, port, (Arg)val);
}

SYSCALL(out) (Cap cap, IoLen len, uintptr_t port, Arg val) {
    return syscall(Syscall::OUT, cap.raw(), (Arg)len, port, val);
}

SYSCALL(out8) (Cap cap, uintptr_t port, uint8_t val) {
    return syscall(Syscall::OUT, cap.raw(), (Arg)IoLen::U8, port, val);
}

SYSCALL(out16) (Cap cap, uintptr_t port, uint16_t val) {
    return syscall(Syscall::OUT, cap.raw(), (Arg)IoLen::U16, port, val);
}

SYSCALL(out32) (Cap cap, uintptr_t port, uint32_t val) {
    return syscall(Syscall::OUT, cap.raw(), (Arg)IoLen::U32, port, val);
}

SYSCALL(out64) (Cap cap, uintptr_t port, uint64_t val) {
    return syscall(Syscall::OUT, cap.raw(), (Arg)IoLen::U64, port, val);
}

enum IpcFlags : Arg {
    NONE = 0,
    SEND = 1 << 0,
    RECV = 1 << 1,
    BLOCK = 1 << 2,
};

FlagsEnum$(IpcFlags);

SYSCALL(ipc) (Cap* cap, Cap dst, Msg* msg, IpcFlags flags = IpcFlags::NONE) {
    return syscall(Syscall::IPC, (Arg)cap, dst.raw(), (Arg)msg, (Arg)flags);
}

} // namespace Hj
