#include <karm-async/task.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>

namespace Karm::Async::Tests {

Task<Ok<>> helloWorld() {
    logInfo("Hello, ");
    co_await Async::sleep(TimeSpan::fromSecs(1));
    logInfo("world!");
    co_return Ok();
}

test$(taskSleep) {
    auto task = helloWorld();
    try$(task.runSync());

    return Ok();
}

Task<Ok<>> taskA() {
    for (int i = 0; i < 10; ++i) {
        logInfo("A: {}", i);
        co_await Async::yield();
    }
    logInfo("A: done");
    co_return Ok();
}

Task<Ok<>> taskB() {
    for (int i = 0; i < 10; ++i) {
        logInfo("B: {}", i);
        co_await Async::yield();
    }
    logInfo("B: done");
    co_return Ok();
}

test$(taskYield) {
    auto tA = taskA();
    auto tB = taskB();
    return Sched::instance().run();
}

Task<Ok<>> taskInner() {
    logInfo("inner A");
    co_await Async::sleep(TimeSpan::fromSecs(1));
    logInfo("inner B");
    co_return Ok();
}

Task<Ok<>> taskOuter() {
    logInfo("outer A");
    co_await taskInner();
    logInfo("outer B");
    co_return Ok();
}

test$(taskNested) {
    auto task = taskOuter();
    try$(task.runSync());
    return Ok();
}

/*
test$(testAll) {
    auto tA = taskA();
    auto tB = taskB();
    Async::all(tA, tB).runSync();
    return Ok();
}
*/

} // namespace Karm::Async::Tests
