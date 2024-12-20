#pragma once

#include <karm-base/string.h>
#include <karm-sys/rpc.h>

namespace Grund::Bus {

struct Locate {
    using Response = Sys::Port;
    String id;
};

} // namespace Grund::Bus
