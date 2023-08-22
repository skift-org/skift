#include <karm-async/async.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>

namespace Karm::Async::Tests {

Task<> helloWorld() {
    co_await after(100_ms);
    co_return Ok();
}

test$(asyncTaskSleep) {
    auto task = helloWorld();
    return loop().run();
}

Task<> taskA() {
    for (int i = 0; i < 10; ++i) {
        co_await after(100_ms);
    }
    co_return Ok();
}

Task<> taskB() {
    for (int i = 0; i < 10; ++i) {
        co_await after(100_ms);
    }
    co_return Ok();
}

test$(asyncTaskYield) {
    auto tA = taskA();
    return loop().run();
}

Task<> taskInner() {
    co_await after(100_ms);
    co_return Ok();
}

Task<> taskOuter() {
    co_await taskInner();
    co_return Ok();
}

test$(asyncTaskNested) {
    auto task = taskOuter();
    return loop().run();
}

test$(asyncTestAll) {
    auto tA = taskA();
    auto tB = taskB();
    auto tAB = all(tA, tB);
    return loop().run();
}

} // namespace Karm::Async::Tests
