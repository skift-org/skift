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

    DOMAIN,
    TASK,
    SPACE,
    MEM,
    IO,
};

enum struct Syscall {
    NONE,

    LOG,

    CREATE_DOMAIN,
    CREATE_TASK,
    CREATE_SPACE,
    CREATE_MEM,
    CREATE_IO,

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

Res<> syscall(Syscall syscall, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0);

Res<> log(char const *msg, size_t len);

Res<> createDomain(Cap dest, Cap *cap, size_t len = 512);

Res<> createTask(Cap dest, Cap *cap, Cap node, Cap space);

Res<> createSpace(Cap dest, Cap *cap);

enum struct MemFlags : Arg {
    NONE = 0,
    LOW = 1 << 0,
    HIGH = 1 << 1,
    DMA = 1 << 2,
};

FlagsEnum$(MemFlags);

Res<> createMem(Cap dest, Cap *cap, uintptr_t phys, size_t len, MemFlags flags = MemFlags::NONE);

Res<> createIo(Cap dest, Cap *cap, uintptr_t base, size_t len);

Res<> drop(Cap cap);

Res<> dup(Cap node, Cap *dst, Cap src);

Res<> start(Cap cap, uintptr_t ip, uintptr_t sp, Args const *args);

Res<> wait(Cap cap, Arg *ret);

Res<> ret(Cap cap, Arg ret);

using MapFlags = Hal::VmmFlags;

Res<> map(Cap cap, uintptr_t *virt, Cap mem, uintptr_t off, size_t len, MapFlags flags = MapFlags::NONE);

Res<> unmap(Cap cap, uintptr_t virt, size_t len);

enum struct IoLen : Arg {
    U8,
    U16,
    U32,
    U64,
};

Res<> in(Cap cap, IoLen len, uintptr_t port, Arg *val);

Res<> out(Cap cap, IoLen len, uintptr_t port, Arg val);

enum IpcFlags : Arg {
    NONE = 0,
    SEND = 1 << 0,
    RECV = 1 << 1,
    BLOCK = 1 << 2,
};

FlagsEnum$(IpcFlags);

Res<> ipc(Cap *cap, Cap dst, Msg *msg, IpcFlags flags = IpcFlags::NONE);

} // namespace Hj
