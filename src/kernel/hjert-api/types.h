#pragma once

#include <hal/vmm.h>
#include <karm-base/array.h>
#include <karm-base/time.h>

namespace Hj {

#define FOREACH_TYPE(TYPE) \
    TYPE(DOMAIN)           \
    TYPE(TASK)             \
    TYPE(SPACE)            \
    TYPE(VMO)              \
    TYPE(IOP)              \
    TYPE(CHANNEL)          \
    TYPE(IRQ)              \
    TYPE(LISTENER)

// clang-format off

enum struct Type {
#define ITER(NAME) NAME,
    FOREACH_TYPE(ITER)
#undef ITER
    _LEN,
};

// clang-format on

static inline char const* toStr(Type type) {
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
    SYSCALL(POLL)

// clang-format off

enum struct Syscall {
#define ITER(NAME) NAME,
    FOREACH_SYSCALL(ITER)
#undef ITER
    _LEN,
};

// clang-format on

static inline Str toStr(Syscall syscall) {
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

enum struct Pledge : u32 {
#define ITER(NAME, VAL) NAME = VAL,
    FOREACH_PLEDGE(ITER)
#undef ITER
    _LEN,
};

// clang-format on

FlagsEnum$(Pledge);

static inline Str toStr(Pledge pledge) {
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

using Arg = usize;

using Args = Array<Arg, 6>;

struct Cap {
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

inline constexpr Cap ROOT = {};

enum struct Sigs : u32 {
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

FlagsEnum$(Sigs);

struct Event {
    Cap cap;
    Sigs sig;
    bool set;
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

inline IoLen bytes2IoLen(usize bytes) {
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

using VmoFlags = Hal::PmmFlags;
using MapFlags = Hal::VmmFlags;

struct DomainProps {
    static constexpr Type TYPE = Type::DOMAIN;
};

struct TaskProps {
    static constexpr Type TYPE = Type::TASK;
    Cap domain;
    Cap space;
};

struct SpaceProps {
    static constexpr Type TYPE = Type::SPACE;
};

struct VmoProps {
    static constexpr Type TYPE = Type::VMO;
    usize phys;
    usize len;
    VmoFlags flags;
};

struct IopProps {
    static constexpr Type TYPE = Type::IOP;
    usize base;
    usize len;
};

struct SentRecv {
    usize bytes;
    usize caps;
};

struct ChannelProps {
    static constexpr Type TYPE = Type::CHANNEL;
    usize bufCap;  //< The capacity of the data buffer (in bytes, must be >= 1)
    usize capsCap; //< The capacity of the caps buffer (in caps, 0 zero means the channel can't carry caps)
};

struct IrqProps {
    static constexpr Type TYPE = Type::IRQ;
    usize irq;
};

struct ListenerProps {
    static constexpr Type TYPE = Type::LISTENER;
};

using _Props = Union<
    DomainProps,
    TaskProps,
    SpaceProps,
    VmoProps,
    IopProps,
    ChannelProps,
    IrqProps,
    ListenerProps>;

struct Props : public _Props {
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
