#pragma once

#include <karm-base/error.h>
#include <karm-base/tuple.h>

#include "types.h"

namespace Hjert::Api {

int doSyscall(void *, size_t);

template <Id _ID, typename Self>
struct Syscall {
    static constexpr Id ID = _ID;

    Karm::Error call() {
        return static_cast<Karm::Error::Code>(doSyscall(this, sizeof(Self)));
    }
};

struct Log : public Syscall<0xb412722092f3afc1, Log> {
    void const *buf;
    size_t len;
};

using Syscalls = Tuple<Log>;

} // namespace Hjert::Api
