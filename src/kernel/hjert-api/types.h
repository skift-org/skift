#pragma once

#include <karm-base/enum.h>
#include <karm-base/std.h>
#include <karm-base/var.h>

namespace Hjert::Api {

using Id = uint64_t;

using Arg = uint64_t;

enum struct IpcFlags : uint64_t {
    BLOCK = 1 << 0,
    SEND = 1 << 1,
    RECV = 1 << 2,
};

FlagsEnum$(IpcFlags);

enum struct MemFlags : uint64_t {
    NIL = 0,

    READ = 1 << 0,
    WRITE = 1 << 1,
    EXEC = 1 << 2,
    USER = 1 << 3,
    LOWER = 1 << 4,
    GLOBAL = 1 << 4,
    DMA = 1 << 5,
};

FlagsEnum$(MemFlags);

enum struct Rights : uint64_t {
    NONE = 0,

    IRQ = 1 << 0,
    DMA = 1 << 1,
    PIO = 1 << 2,
    LOG = 1 << 3,
    TASK = 1 << 4,
    MEM = 1 << 5,
    TIME = 1 << 6,
    STRONG = 1 << 7,
    SHARE = 1 << 8,
};

FlagsEnum$(Rights);

struct Event {
    enum { IRQ } type;
    uint64_t data;
};

struct Msg {
    Id from;
    Id to;

    union {
        uint8_t buf[sizeof(uint64_t) * 5];
        uint64_t args[5];
        Event event;
    };
};

struct TaskProps {
    Id space;
    Rights rights;
};

struct MemProps {
    uintptr_t addr;
    size_t size;
    MemFlags flags;
};

struct SpaceProps {};

using Props = Var<
    TaskProps,
    SpaceProps,
    MemProps>;

} // namespace Hjert::Api
