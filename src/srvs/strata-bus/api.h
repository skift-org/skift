#pragma once

import Karm.Core;
import Karm.Sys;

using namespace Karm;

namespace Strata::Bus::Api {

struct Locate {
    using Response = Sys::Port;
    String id;
};

struct Listen {
    Meta::Id mid;
};

struct Start {
    // FIXME: Handle intent instead of simple id?
    String id;
};

} // namespace Strata::Bus::Api
