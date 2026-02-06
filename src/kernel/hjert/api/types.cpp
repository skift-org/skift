module;

#include <hal/vmm.h>

export module Hjert.Api:types;

import Karm.Core;

namespace Hj {

export enum struct Mode : u8 {
    EMBRYO, // The task is being created
    IDLE,   // The task is only run when there is no other task to run
    USER,   // The task is running in user mode
    SUPER,  // The task is running in supervisor mode propably serving a syscall
};

#define FOREACH_TYPE(TYPE) \
    TYPE(DOMAIN)           \
    TYPE(TASK)             \
    TYPE(SPACE)            \
    TYPE(VMO)              \
    TYPE(IOP)              \
    TYPE(CHANNEL)          \
    TYPE(IRQ)              \
    TYPE(LISTENER)         \
    TYPE(PIPE)

// clang-format off

export enum struct Type {
#define ITER(NAME) NAME,
    FOREACH_TYPE(ITER)
#undef ITER
    _LEN,
};

// clang-format on

export Str toStr(Type type) {
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
    SYSCALL(NOW)                 \
    SYSCALL(LOG)                 \
    SYSCALL(CREATE)              \
    SYSCALL(LABEL)               \
    SYSCALL(DROP)                \
    SYSCALL(PLEDGE)              \
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
    SYSCALL(LISTEN)              \
    SYSCALL(POLL)                \
    SYSCALL(READ)                \
    SYSCALL(WRITE)               \
    SYSCALL(BIND)                \
    SYSCALL(EOI)

// clang-format off

export enum struct Syscall {
#define ITER(NAME) NAME,
    FOREACH_SYSCALL(ITER)
#undef ITER
    _LEN,
};

// clang-format on

export Str toStr(Syscall syscall) {
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

#define FOREACH_PLEDGE(PLEDGE) \
    PLEDGE(HW, 1 << 0)         \
    PLEDGE(TASK, 1 << 1)       \
    PLEDGE(MEM, 1 << 2)        \
    PLEDGE(LOG, 1 << 3)        \
    PLEDGE(ALL, HW | TASK | MEM | LOG)

// clang-format off

export enum struct Pledge : u32 {
#define ITER(NAME, VAL) NAME = VAL,
    FOREACH_PLEDGE(ITER)
#undef ITER
    _LEN,
};

// clang-format on

export Str toStr(Pledge pledge) {
    switch (pledge) {
#define ITER(NAME, VAL) \
    case Pledge::NAME:  \
        return #NAME;
        FOREACH_PLEDGE(ITER)
#undef ITER
    default:
        panic("invalid right");
    }
}

export using Arg = usize;

export using Args = Array<Arg, 6>;

export struct Cap {
    Arg _raw = 0;

    static constexpr usize SHIFT = 11;
    static constexpr usize LEN = 1 << 11;
    static constexpr usize MASK = LEN - 1;

    constexpr Cap() = default;

    constexpr explicit Cap(Arg raw)
        : _raw(raw) {}

    constexpr Arg raw() const {
        return _raw;
    }

    constexpr bool isRoot() const {
        return _raw == 0;
    }

    explicit operator bool() const {
        return _raw != 0;
    }

    std::strong_ordering operator<=>(Cap const& other) const = default;

    usize slot() const {
        auto curr = _raw & MASK;
        auto upper = _raw >> SHIFT;
        while (upper) {
            curr = upper & MASK;
            upper >>= SHIFT;
        }
        return curr;
    }
};

export constexpr Cap ROOT = {};

export enum struct Sigs : u32 {
    NONE = 0,

    READABLE = 1uz << 0,
    WRITABLE = 1uz << 1,
    CLOSED = 1uz << 2,
    EXITED = 1uz << 3,
    CRASHED = 1uz << 4,
    TRIGGERED = 1uz << 5,

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

export struct Event {
    Cap cap;
    Flags<Sigs> sigs;
    bool set;
};

export enum struct IoLen : Arg {
    U8,
    U16,
    U32,
    U64,
};

export usize ioLen2Bytes(IoLen len) {
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

export IoLen bytes2IoLen(usize bytes) {
    switch (bytes) {
    case 1:
        return IoLen::U8;
    case 2:
        return IoLen::U16;
    case 4:
        return IoLen::U32;
    case 8:
        return IoLen::U64;
    }
    return IoLen::U8;
}

export using VmoFlags = Hal::PmmFlags;
export using MapFlags = Hal::VmmFlags;

export struct DomainProps {
    static constexpr Type TYPE = Type::DOMAIN;
};

export struct TaskProps {
    static constexpr Type TYPE = Type::TASK;
    Cap domain;
    Cap space;
};

export struct SpaceProps {
    static constexpr Type TYPE = Type::SPACE;
};

export struct VmoProps {
    static constexpr Type TYPE = Type::VMO;
    usize phys;
    usize len;
    Flags<VmoFlags> flags;
};

export struct IopProps {
    static constexpr Type TYPE = Type::IOP;
    usize base;
    usize len;
};

export struct SentRecv {
    usize bytes;
    usize caps;
};

export struct ChannelProps {
    static constexpr Type TYPE = Type::CHANNEL;
    usize bufCap;  //< The capacity of the data buffer (in bytes, must be >= 1)
    usize capsCap; //< The capacity of the caps buffer (in caps, 0 zero means the channel can't carry caps)
};

export struct IrqProps {
    static constexpr Type TYPE = Type::IRQ;
    usize irq;
};

export struct ListenerProps {
    static constexpr Type TYPE = Type::LISTENER;
};

export struct PipeProps {
    static constexpr Type TYPE = Type::PIPE;
    usize bufCap; //< The capacity of the data buffer (in bytes, must be >= 1)
};

export using _Props = Union<
    DomainProps,
    TaskProps,
    SpaceProps,
    VmoProps,
    IopProps,
    ChannelProps,
    IrqProps,
    ListenerProps,
    PipeProps>;

export struct Props : _Props {
    using _Props::_Props;

    Type type() const {
        return visit([](auto const& props) {
            return props.TYPE;
        });
    }

    Str typeStr() const {
        return toStr(type());
    }
};

} // namespace Hj
