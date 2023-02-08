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
    NONE,

    TASK,
    MEM,
    SPACE,
    MUTEXT,
};

/* --- Handles -------------------------------------------------------------- */

struct Task;
struct Mem;
struct Io;
struct Space;
struct Mutex;

using RawHandle = uint64_t;

static const RawHandle SELF = 0xffffffffffffffff;

template <typename T>
struct Handle {
    RawHandle _handle;

    Handle(RawHandle handle) : _handle(handle) {}
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
struct Props<Io> {
    uint16_t port;
    size_t size;
};

template <>
struct Props<Mutex> {
};

template <>
struct Infos<Mem> {
    uintptr_t vaddr;
    uintptr_t paddr;
    size_t size;
    MemFlags flags;
};

template <>
struct Infos<Io> {
    uint16_t port;
    size_t size;
};

template <>
struct Props<Space> {
    SpaceFlags flags;
};

template <>
struct Infos<Space> {
    SpaceFlags flags;
};

template <>
struct Infos<Mutex> {
    bool locked;
};

using AnyHandle = Var<Handle<Task>, Handle<Mem>, Handle<Space>>;
using AnyProps = Var<Props<Task>, Props<Mem>, Props<Io>, Props<Space>, Props<Mutex>>;
using AnyInfos = Var<Infos<Task>, Infos<Mem>, Infos<Io>, Infos<Space>, Infos<Mutex>>;

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
