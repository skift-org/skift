#include <karm-async/one.h>
#include <karm-async/promise.h>
#include <karm-async/run.h>
#include <karm-async/task.h>
#include <karm-test/macros.h>

namespace Karm::Async::Tests {

Async::_Task<int> taskValue() {
    co_return 42;
}

test$("karm-async-task-value") {
    auto res = Async::run(taskValue());
    expectEq$(res, 42);
    return Ok();
}

Async::_Task<int> taskOuter() {
    co_return co_await taskValue();
}

test$("karm-async-task-outer") {
    auto res = Async::run(taskOuter());
    expectEq$(res, 42);
    return Ok();
}

test$("karm-async-task-detach") {
    int res = 0xdead;
    Async::detach(taskValue(), [&](int r) {
        res = r;
    });
    expectEq$(res, 42);
    return Ok();
}

} // namespace Karm::Async::Tests
