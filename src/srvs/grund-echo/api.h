#pragma once

#include <karm-base/string.h>

namespace Grund::Echo::Api {

struct Request {
    using Response = String;
    String msg;
};

} // namespace Grund::Echo::Api
