module;

export module Strata.Protos:ibus;

import Karm.Core;
import Karm.Sys;

using namespace Karm;

namespace Strata::IBus {

export struct Locate {
    using Response = Sys::Port;
    String id;
};

export struct Listen {
    Meta::Id mid;
};

export struct Start {
    // FIXME: Handle intent instead of simple id?
    String id;
};

} // namespace Strata::IBus
