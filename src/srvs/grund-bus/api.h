#pragma once

#include <karm-base/string.h>
#include <karm-rpc/base.h>

namespace Grund::Bus::Api {

struct Locate {
    using Response = Rpc::Port;
    String id;
};

struct Listen {
    Meta::Id mid;
};

struct Start {
    // FIXME: Handle intent instead of simple id?
    String id;
};

} // namespace Grund::Bus::Api
