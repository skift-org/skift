#pragma once

#include <hal/vmm.h>
#include <karm-base/array.h>
#include <karm-base/enum.h>
#include <karm-base/macros.h>
#include <karm-base/res.h>

namespace Hj {

/* --- Basic Data Types ----------------------------------------------------- */

#define FOREACH_TYPE(TYPE) \
    TYPE(NONE)             \
    TYPE(DOMAIN)           \
    TYPE(TASK)             \
    TYPE(SPACE)            \
    TYPE(VMO)              \
    TYPE(IO)

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
    SYSCALL(LABEL)               \
    SYSCALL(DROP)                \
    SYSCALL(DUP)                 \
    SYSCALL(START)               \
    SYSCALL(WAIT)                \
    SYSCALL(RET)                 \
    SYSCALL(MAP)                 \
    SYSCALL(UNMAP)               \
    SYSCALL(IN)                  \
    SYSCALL(OUT)                 \
    SYSCALL(IPC)

// clang-format off
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

enum MsgFlags : Arg {
    NONE = 0,

    CAP0 = 1 << 0,
    CAP1 = 1 << 1,
    CAP2 = 1 << 2,
    CAP3 = 1 << 3,
    CAP4 = 1 << 4,
    CAP5 = 1 << 5,
};

FlagsEnum$(MsgFlags);

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

/* --- Syscall Interface ---------------------------------------------------- */

Res<> _syscall(Syscall syscall, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0);

Res<> _log(char const *msg, usize len);

Res<> _createDomain(Cap dest, Cap *cap);

Res<> _createTask(Cap dest, Cap *cap, Cap node, Cap space);

Res<> _createSpace(Cap dest, Cap *cap);

enum struct VmoFlags : Arg {
    NONE = 0,
    LOW = 1 << 0,
    HIGH = 1 << 1,
    DMA = 1 << 2,
};

FlagsEnum$(VmoFlags);

Res<> _createVmo(Cap dest, Cap *cap, usize phys, usize len, VmoFlags flags = VmoFlags::NONE);

Res<> _createIo(Cap dest, Cap *cap, usize base, usize len);

Res<> _label(Cap cap, char const *label, usize len);

Res<> _drop(Cap cap);

Res<> _dup(Cap node, Cap *dst, Cap src);

Res<> _start(Cap cap, usize ip, usize sp, Args const *args);

Res<> _wait(Cap cap, Arg *ret);

Res<> _ret(Cap cap, Arg ret);

using MapFlags = Hal::VmmFlags;

Res<> _map(Cap cap, usize *virt, Cap vmo, usize off, usize len, MapFlags flags = MapFlags::NONE);

Res<> _unmap(Cap cap, usize virt, usize len);

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

Res<> _in(Cap cap, IoLen len, usize port, Arg *val);

Res<> _out(Cap cap, IoLen len, usize port, Arg val);

enum struct IpcFlags : u32 {
    NONE = 0,
    SEND = 1 << 0,
    RECV = 1 << 1,
    BLOCK = 1 << 2,
};

FlagsEnum$(IpcFlags);

Res<> _ipc(Cap *cap, Cap dst, Msg *msg, IpcFlags flags = IpcFlags::NONE);

} // namespace Hj
