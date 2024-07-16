#pragma once

#include <karm-base/std.h>

namespace Karm::Ipc {

struct Header {
    u64 from, to;
    u64 oid, uid, mid, seq;
};

} // namespace Karm::Ipc
