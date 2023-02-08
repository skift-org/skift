#pragma once

#include <karm-base/array.h>
#include <karm-base/error.h>
#include <karm-base/range.h>
#include <karm-base/result.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>

#include "types.h"

namespace Hjert::Api {

/* --- Common --------------------------------------------------------------- */

using SyscallId = uint64_t;

inline int doSyscall(SyscallId id, void *args) {
    uint64_t res;
    asm volatile("syscall"
                 : "=a"(res)
                 : "a"(id), "b"(args)
                 : "memory", "r11", "rcx");
    return res;
}

template <uint64_t _ID, typename Crtp>
struct Syscall {
    static constexpr SyscallId ID = _ID;

    Karm::Error call() {
        return static_cast<Karm::Error::Code>(doSyscall(ID, this));
    }

    Karm::Error operator()() {
        return call();
    }

    template <typename... Args>
    static Karm::Result<Crtp> call(Args &&...args) {
        Crtp payload = {{}, std::forward<Args>(args)...};
        return payload.call();
    }
};

/* --- Objects & Handles ---------------------------------------------------- */

struct Create : public Syscall<0x990cabbbd5688039, Create> {
    AnyHandle handle;
    AnyProps props;
};

struct Grant : public Syscall<0xad95f0fd00e9019c, Grant> {
    Handle<Task> target;
    AnyHandle what;
    Rights rights;
};

struct Ref : public Syscall<0x1903948e3ecd5acf, Ref> {
    AnyHandle handle;
};

struct Deref : public Syscall<0xa6f367203f132864, Deref> {
    AnyHandle handle;
};

/* --- Tasks ---------------------------------------------------------------- */

struct Start : public Syscall<0x806a5078fe85820c, Start> {
    Handle<Task> task;
    size_t ip;
    size_t sp;
    Array<Arg, 5> args;
};

struct Exit : public Syscall<0x19fb41a759bb00c2, Exit> {
    Handle<Task> handle;
    int code;
};

struct Wait : public Syscall<0x254d34110091d4a6, Wait> {
    Handle<Task> handle;
    int code;
};

struct Pledge : public Syscall<0xaf10a51c36216dc6, Pledge> {
    Handle<Task> handle;
    Rights rights;
};

/* --- Mutexes -------------------------------------------------------------- */

struct Lock : public Syscall<0x655750101d2d4355, Lock> {
    Handle<Mutex> handle;
};

struct Unlock : public Syscall<0x2f08fd833d42cc9f, Unlock> {
    Handle<Mutex> handle;
};

/* --- Memory --------------------------------------------------------------- */

struct Map : public Syscall<0xfe32feecc3ccc7a4, Map> {
    Handle<Mem> mem;
    Handle<Space> space;

    size_t offset;
    size_t size;
    size_t vaddr;
    MemFlags flags;
};

struct Unmap : public Syscall<0x72f6f47a9adc1467, Unmap> {
    Handle<Space> space;

    size_t vaddr;
    size_t size;
};

/* --- Ipc & Events --------------------------------------------------------- */

struct Ipc : public Syscall<0xd975ebbfbe37ccd5, Ipc> {
    using enum IpcFlags;

    Msg msg;
    IpcFlags flags;
};

struct Bind : public Syscall<0xa582f8971e5fa3ad, Bind> {
    Event event;
};

struct Unbind : public Syscall<0x359c377c55df6bbb, Unbind> {
    Event event;
};

struct Ack : public Syscall<0x6135aa8956303523, Ack> {
    Event event;
};

/* --- I/O ------------------------------------------------------------------ */

struct Io : public Syscall<0x620da3889c470923, Io> {
    uint16_t port;
    uint8_t size;
    uint64_t data;
    bool write;
};

/* --- Misc ----------------------------------------------------------------- */

struct Log : public Syscall<0x83da4dc5b08d71b3, Log> {
    char const *buf;
    size_t len;
};

struct Now : public Syscall<0x2e5b6ef33d0b5f91, Now> {
    TimeStamp stamp;
};

using Syscalls = Tuple<
    Create, Grant, Ref, Deref,
    Start, Exit, Pledge,
    Map, Unmap,
    Ipc, Bind, Unbind, Ack,
    Io,
    Log, Now>;

} // namespace Hjert::Api
