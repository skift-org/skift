import Karm.Core;

#include <karm-test/macros.h>

namespace Karm::Async::Tests {

Async::_Task<int> taskValue() {
    co_return 42;
}

test$("task-value") {
    auto res = Async::run(taskValue());
    expectEq$(res, 42);
    return Ok();
}

Async::_Task<int> taskOuter() {
    co_return co_await taskValue();
}

test$("task-outer") {
    auto res = Async::run(taskOuter());
    expectEq$(res, 42);
    return Ok();
}

test$("task-detach") {
    int res = 0xdead;
    Async::detach(taskValue(), [&](int r) {
        res = r;
    });
    expectEq$(res, 42);
    return Ok();
}

} // namespace Karm::Async::Tests
