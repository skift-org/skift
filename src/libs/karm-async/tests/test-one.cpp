#include <karm-async/one.h>
#include <karm-async/task.h>
#include <karm-test/macros.h>

namespace Karm::Async::Tests {

test$("karm-async-sender-one") {
    auto sender = Async::One<int>{10};
    auto res = Async::run(sender);
    expectEq$(res, 10);
    return Ok();
}

} // namespace Karm::Async::Tests
