#pragma once

#include <karm-base/enum.h>
#include <karm-base/std.h>
#include <karm-base/var.h>

namespace Hjert::Api {

/* --- Constants ----------------------------------------------------------- */

enum struct IpcFlags : uint64_t {
    BLOCK = 1 << 0,
    SEND = 1 << 1,
    RECV = 1 << 2,
};

FlagsEnum$(IpcFlags);

enum struct TaskFlags : uint64_t {
    NONE = 0,

    USER = 1 << 0,
};

FlagsEnum$(TaskFlags);

enum struct MemFlags : uint64_t {
    NONE = 0,

    READ = 1 << 0,
    WRITE = 1 << 1,
    EXEC = 1 << 2,
    USER = 1 << 3,
    LOWER = 1 << 4,
    GLOBAL = 1 << 4,
    DMA = 1 << 5,
};

FlagsEnum$(MemFlags);

enum struct SpaceFlags : uint64_t {
    NONE = 0,
};

FlagsEnum$(SpaceFlags)

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

enum struct Type {
    TASK,
    MEM,
    SPACE
};

/* --- Handles -------------------------------------------------------------- */

struct Task;
struct Mem;
struct Space;

using RawHandle = uint64_t;

template <typename T>
struct Handle {
    RawHandle _handle;

    Handle(RawHandle);

    Handle(Handle &);

    Handle(Handle &&);
};

template <typename T>
struct Props;

template <typename T>
struct Infos;

template <>
struct Props<Task> {
    Handle<Space> space;
    Rights rights;
    TaskFlags flags;
};

template <>
struct Infos<Task> {
    TaskFlags flags;
};

template <>
struct Props<Mem> {
    uintptr_t addr;
    size_t size;
    MemFlags flags;
};

template <>
struct Infos<Mem> {
    uintptr_t vaddr;
    uintptr_t paddr;
    size_t size;
    MemFlags flags;
};

template <>
struct Props<Space> {
    SpaceFlags flags;
};

template <>
struct Infos<Space> {
    SpaceFlags flags;
};

using AnyHandle = Var<Handle<Task>, Handle<Mem>, Handle<Space>>;
using AnyProps = Var<Props<Task>, Props<Mem>, Props<Space>>;
using AnyInfos = Var<Infos<Task>, Infos<Mem>, Infos<Space>>;

using Arg = uint64_t;

struct Event {
    enum { IRQ } type;
    uint64_t data;
};

struct Msg {
    Handle<Task> from;
    Handle<Task> to;

    union {
        uint8_t buf[sizeof(uint64_t) * 5];
        uint64_t args[5];
        Event event;
    };
};

} // namespace Hjert::Api
