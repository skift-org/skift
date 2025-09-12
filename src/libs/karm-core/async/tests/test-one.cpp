import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Async::Tests {

test$("sender-one") {
    auto sender = Async::One<int>{10};
    auto res = Async::run(sender);
    expectEq$(res, 10);
    return Ok();
}

} // namespace Karm::Async::Tests
