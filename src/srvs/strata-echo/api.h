#pragma once

#include <karm-base/string.h>

namespace Strata::Echo::Api {

struct Request {
    using Response = String;
    String msg;
};

} // namespace Strata::Echo::Api
