#pragma once

#include <karm-base/std.h>

namespace Hjert::Api {

using Id = uint64_t;

using Arg = uint64_t;

struct Message {
    uint64_t flags;
    Id from;
    Id to;

    union {
        Arg args[5];
        uint8_t buf[sizeof(args)];
    };
};

} // namespace Hjert::Api
