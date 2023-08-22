#include <karm-async/async.h>
#include <karm-logger/logger.h>
#include <karm-test/macros.h>

namespace Karm::Async::Tests {

Task<> helloWorld() {
    logInfo("Hello, ");
    co_await after(100_ms);
    logInfo("world!");
    co_return Ok();
}

test$(taskSleep) {
    auto task = helloWorld();
    return loop().run();
}

Task<> taskA() {
    for (int i = 0; i < 10; ++i) {
        co_await after(100_ms);
        logInfo("A: {}", i);
    }
    logInfo("A: done");
    co_return Ok();
}

Task<> taskB() {
    for (int i = 0; i < 10; ++i) {
        logInfo("B: {}", i);
        co_await after(100_ms);
    }
    logInfo("B: done");
    co_return Ok();
}

test$(taskYield) {
    auto tA = taskA();
    return loop().run();
}

Task<> taskInner() {
    logInfo("inner A");
    co_await after(100_ms);
    logInfo("inner B");
    co_return Ok();
}

Task<> taskOuter() {
    logInfo("outer A");
    co_await taskInner();
    logInfo("outer B");
    co_return Ok();
}

test$(taskNested) {
    auto task = taskOuter();
    return loop().run();
}

test$(testAll) {
    auto tA = taskA();
    auto tB = taskB();
    auto tAB = all(tA, tB);
    return loop().run();
}

} // namespace Karm::Async::Tests
