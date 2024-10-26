#include <karm-base/async.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$("karm-base-sender-one") {
    auto sender = Async::One<int>{10};
    auto res = Async::run(sender);
    expectEq$(res, 10);
    return Ok();
}

Async::_Task<int> taskValue() {
    co_return 42;
}

test$("karm-base-task-value") {
    auto res = Async::run(taskValue());
    expectEq$(res, 42);
    return Ok();
}

Async::_Task<int> taskOuter() {
    co_return co_await taskValue();
}

test$("karm-base-task-outer") {
    auto res = Async::run(taskOuter());
    expectEq$(res, 42);
    return Ok();
}

test$("karm-base-task-detach") {
    int res = 0xdead;
    Async::detach(taskValue(), [&](int r) {
        res = r;
    });
    expectEq$(res, 42);
    return Ok();
}

test$("karm-base-promise-one-future") {
    Opt<Async::_Future<int>> future;
    {
        Async::_Promise<int> promise;
        future = promise.future();
        promise.resolve(42);
    }
    auto res = Async::run(*future);
    expectEq$(res, 42);
    return Ok();
}

test$("karm-base-promise-multiple-futures") {
    Opt<Async::_Future<int>> f1, f2, f3;
    {
        Async::_Promise<int> promise;

        f1 = promise.future();
        f2 = promise.future();
        f3 = promise.future();

        promise.resolve(42);
    }

    auto res1 = Async::run(*f1);
    auto res2 = Async::run(*f2);
    auto res3 = Async::run(*f3);

    expectEq$(res1, 42);
    expectEq$(res2, 42);
    expectEq$(res3, 42);

    return Ok();
}

} // namespace Karm::Base::Tests
