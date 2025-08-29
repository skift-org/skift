#pragma once

import Karm.Core;

using namespace Karm;

namespace Strata::Echo::Api {

struct Request {
    using Response = String;
    String msg;
};

} // namespace Strata::Echo::Api
