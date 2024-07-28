#pragma once

#include <karm-base/reflect.h>
#include <karm-base/string.h>

namespace Grund::Echo {

struct Request {
    using Response = String;
    String msg;
};

} // namespace Grund::Echo

Reflectable$(Grund::Echo::Request, msg);
