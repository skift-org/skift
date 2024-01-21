#include <karm-base/async.h>
#include <karm-test/macros.h>

namespace Karm::Base::Tests {

test$(testSenderOne) {
    auto sender = Async::One<int>{10};
    auto res = Async::run(sender);
    expectEq$(res, 10);
    return Ok();
}

Async::_Task<int> taskValue() {
    co_return 42;
}

test$(testTaskValue) {
    auto res = Async::run(taskValue());
    expectEq$(res, 42);
    return Ok();
}

Async::_Task<int> taskOuter() {
    co_return co_await taskValue();
}

test$(testTaskOuter) {
    auto res = Async::run(taskOuter());
    expectEq$(res, 42);
    return Ok();
}

test$(testTaskDetach) {
    int res = 0xdead;
    Async::detach(taskValue(), [&](int r) {
        res = r;
    });
    expectEq$(res, 42);
    return Ok();
}

test$(testPromiseOneFuture) {
    Opt<Async::_Future<int>> future;
    {
        Async::_Promise<int> promise;
        future = promise.future();
        promise.resolve(42);
    }
    auto res = Async::run(future->wait());
    expectEq$(res, 42);
    return Ok();
}

test$(testPromiseMultipleFutures) {
    Opt<Async::_Future<int>> f1, f2, f3;
    {
        Async::_Promise<int> promise;

        f1 = promise.future();
        f2 = promise.future();
        f3 = promise.future();

        promise.resolve(42);
    }

    auto res1 = Async::run(f1->wait());
    auto res2 = Async::run(f2->wait());
    auto res3 = Async::run(f3->wait());

    expectEq$(res1, 42);
    expectEq$(res2, 42);
    expectEq$(res3, 42);

    return Ok();
}

} // namespace Karm::Base::Tests
