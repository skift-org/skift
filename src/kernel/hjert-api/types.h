#pragma once

#include <hal/vmm.h>
#include <karm-base/array.h>
#include <karm-base/time.h>

namespace Hj {

#define FOREACH_TYPE(TYPE) \
    TYPE(NONE)             \
    TYPE(DOMAIN)           \
    TYPE(TASK)             \
    TYPE(SPACE)            \
    TYPE(VMO)              \
    TYPE(IO)               \
    TYPE(CHANNEL)          \
    TYPE(IRQ)

// clang-format off
enum struct Type {
#define ITER(NAME) NAME,
    FOREACH_TYPE(ITER)
    _LEN,
#undef ITER
};
// clang-format on

static inline char const *toStr(Type type) {
    switch (type) {
#define ITER(NAME)   \
    case Type::NAME: \
        return #NAME;
        FOREACH_TYPE(ITER)
#undef ITER
    default:
        panic("invalid type");
    }
}

#define FOREACH_SYSCALL(SYSCALL) \
    SYSCALL(NONE)                \
    SYSCALL(LOG)                 \
    SYSCALL(CREATE_DOMAIN)       \
    SYSCALL(CREATE_TASK)         \
    SYSCALL(CREATE_SPACE)        \
    SYSCALL(CREATE_VMO)          \
    SYSCALL(CREATE_IO)           \
    SYSCALL(CREATE_CHANNEL)      \
    SYSCALL(CREATE_IRQ)          \
    SYSCALL(LABEL)               \
    SYSCALL(DROP)                \
    SYSCALL(DUP)                 \
    SYSCALL(START)               \
    SYSCALL(MAP)                 \
    SYSCALL(UNMAP)               \
    SYSCALL(IN)                  \
    SYSCALL(OUT)                 \
    SYSCALL(SEND)                \
    SYSCALL(RECV)                \
    SYSCALL(CLOSE)               \
    SYSCALL(SIGNAL)              \
    SYSCALL(WAIT)

// clang-format offR
enum struct Syscall {
#define ITER(NAME) NAME,
    FOREACH_SYSCALL(ITER)
        _LEN,
#undef ITER
};
// clang-format on

static inline char const *toStr(Syscall syscall) {
    switch (syscall) {
#define ITER(NAME)      \
    case Syscall::NAME: \
        return #NAME;
        FOREACH_SYSCALL(ITER)
#undef ITER
    default:
        panic("invalid syscall");
    }
}

using Arg = usize;

using Args = Array<Arg, 6>;

struct Cap {
    Arg _raw;

    constexpr Cap() = default;

    constexpr Cap(Arg raw)
        : _raw(raw) {}

    constexpr Arg raw() const {
        return _raw;
    }

    constexpr bool isRoot() const {
        return _raw == 0;
    }

    operator bool() const {
        return _raw != 0;
    }
};

inline constexpr Cap ROOT = {};

enum struct MsgFlags : Arg {
    NONE = 0,

    CAP0 = 1 << 0,
    CAP1 = 1 << 1,
    CAP2 = 1 << 2,
    CAP3 = 1 << 3,
    CAP4 = 1 << 4,
    CAP5 = 1 << 5,
};

FlagsEnum$(MsgFlags);

struct Msg {
    Arg label;
    Arg flags{};
    Args args{};

    constexpr Msg(Arg label)
        : label(label) {}

    void store(usize idx, Arg arg) {
        flags &= ~(1 << idx);
        args[idx] = arg;
    }

    void store(usize idx, Cap cap) {
        flags |= 1 << idx;
        args[idx] = cap.raw();
    }
};

enum struct Signals : Arg {
    NONE = 0,

    READABLE = 1uz << 0,
    WRITABLE = 1uz << 1,
    CLOSED = 1uz << 2,
    EXITED = 1uz << 3,

    // User defined signals.

    USER0 = 1uz << 24,
    USER1 = 1uz << 25,
    USER2 = 1uz << 26,
    USER3 = 1uz << 27,
    USER4 = 1uz << 28,
    USER5 = 1uz << 29,
    USER6 = 1uz << 30,
    USER7 = 1uz << 31,
};

FlagsEnum$(Signals);

struct Cond {
    Cap cap;
    Signals set;
    Signals unset;
};

struct Event {
    Cap cap;
    Signals set;
    Signals unset;
    Arg val;
};

enum struct IoLen : Arg {
    U8,
    U16,
    U32,
    U64,
};

inline usize ioLen2Bytes(IoLen len) {
    switch (len) {
    case IoLen::U8:
        return 1;
    case IoLen::U16:
        return 2;
    case IoLen::U32:
        return 4;
    case IoLen::U64:
        return 8;
    }
    return 0;
}

using VmoFlags = Hal::PmmFlags;
using MapFlags = Hal::VmmFlags;

} // namespace Hj
