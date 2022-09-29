#pragma once

#include <karm-base/error.h>
#include <karm-base/result.h>
#include <karm-base/tuple.h>
#include <karm-time/time.h>

#include "types.h"

namespace Hjert::Api {

/* --- Common --------------------------------------------------------------- */

int doSyscall(void *, size_t);

template <Id _ID, typename Self>
struct Syscall {
    static constexpr Id ID = _ID;

    Karm::Error call() {
        return static_cast<Karm::Error::Code>(doSyscall(this, sizeof(Self)));
    }

    Karm::Error operator()() {
        return call();
    }

    template <typename... Args>
    static Karm::Result<Self> call(Args &&...args) {
        Self self{std::forward<Args>(args)...};
        return self.call();
    }
};

/* --- Objects & Handles ---------------------------------------------------- */

struct Create : public Syscall<0x1c1c1c1c1c1c1c1c, Create> {
    Id id;
    Props props;
};
struct Grant : public Syscall<0x2c2c2c2c2c2c2c2c, Grant> {
    Id target;
    Id what;
    Rights rights;
};

struct Ref : public Syscall<0x3c3c3c3c3c3c3c3c, Ref> {
    Id id;
};

struct Deref : public Syscall<0x4c4c4c4c4c4c4c4c, Deref> {
    Id id;
};

/* --- Tasks ---------------------------------------------------------------- */

struct Start : public Syscall<0x3d3d3d3d3d3d3d3d, Start> {
    Id id;
    size_t ip;
    size_t sp;
    Arg args[5];
};

struct Exit : public Syscall<0x3e3e3e3e3e3e3e3e, Exit> {
    int code;
};

struct Wait : public Syscall<0x4e4e4e4e4e4e4e4e, Wait> {
    Id id;
    int code;
};

struct Pledge : public Syscall<0x5e5e5e5e5e5e5e5e, Pledge> {
    Rights rights;
};

/* --- Memory --------------------------------------------------------------- */

struct Map : public Syscall<0x2d2d2d2d2d2d2d2d, Map> {
    Id mem;
    Id space;

    size_t offset;
    size_t size;
    size_t vaddr;
    MemFlags flags;
};

struct Unmap : public Syscall<0x3e3e3e3e3e3e3e3e, Unmap> {
    Id space;

    size_t vaddr;
    size_t size;
};

/* --- Ipc & Events --------------------------------------------------------- */

struct Ipc : public Syscall<0x2b0b0b0b0b0b0b0b, Ipc> {
    using enum IpcFlags;

    Msg msg;
    IpcFlags flags;
};

struct Bind : public Syscall<0x2b1b1b1b1b1b1b1b, Bind> {
    Event event;
};

struct Unbind : public Syscall<0x2b2b2b2b2b2b2b2b, Unbind> {
    Event event;
};

struct Ack : public Syscall<0x2b3b3b3b3b3b3b3b, Ack> {
    Event event;
};

/* --- I/O ------------------------------------------------------------------ */

struct Io : public Syscall<0x2a0a0a0a0a0a0a0a, Io> {
    uint16_t port;
    uint8_t size;
    uint64_t data;
    bool write;
};

/* --- Misc ----------------------------------------------------------------- */

struct Log : public Syscall<0xb412722092f3afc1, Log> {
    char const *buf;
    size_t len;
};

struct Now : public Syscall<0x1b0b0b0b0b0b0b0b, Now> {
    Time::Stamp stamp;
};

using Syscalls = Tuple<
    Create, Grant, Ref, Deref,
    Start, Exit, Pledge,
    Map, Unmap,
    Ipc, Bind, Unbind, Ack,
    Io,
    Log, Now>;

} // namespace Hjert::Api
