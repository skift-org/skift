#pragma once

#include <karm-base/base.h>

namespace Karm::Ipc {

struct Header {
    u64 from, to;
    u64 oid, uid, mid, seq;
};

} // namespace Karm::Ipc
