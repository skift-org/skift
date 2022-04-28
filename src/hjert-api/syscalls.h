#pragma once

#include <karm-base/error.h>

#include "types.h"

namespace Hjert::Api {

int do_syscall(void *, size_t);

template <Id id, typename Self>
struct Syscall {
    static constexpr Id ID = id;

    auto call() -> Karm::Base::Error {
        return static_cast<Karm::Base::Error::Code>(do_syscall(this, sizeof(Self)));
    }
};

struct Log : Syscall<0xb412722092f3afc1, Log> {
    char const *buf;
    size_t len;
};

} // namespace Hjert::Api
