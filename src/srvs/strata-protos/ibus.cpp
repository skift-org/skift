module;

export module Strata.Protos:ibus;

import Karm.Core;
import Karm.Sys;
import Karm.Ref;

using namespace Karm;

namespace Strata::IBus {

export struct Connect {
    Opt<Rc<Sys::Fd>> fd;
    Ref::Url url;
};

export struct Incoming {
    Opt<Rc<Sys::Fd>> fd;
};

export struct Launch {
    Ref::Url url;
};

} // namespace Strata::IBus
