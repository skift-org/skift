#include <karm-sys/async.h>
#include <karm-test/macros.h>

namespace Karm::Sys::Tests {

Task<usize> taskValue() {
    co_return Ok(42);
}

test$(asyncTaskValue) {
    return try$(runSync([&]() -> Task<> {
        auto task = taskValue();
        // Don't co_await here, because we want to test the case
        // where the task is return before it is awaited.
        co_expectEq$(co_try_await$(task), 42uz);
        co_return Ok();
    }()));
}

Task<usize> taskParam(usize value) {
    co_return Ok(value);
}

test$(asyncTaskParam) {
    return try$(runSync([&]() -> Task<> {
        auto task = taskParam(42);
        co_expectEq$(co_try_await$(task), 42uz);
        co_return Ok();
    }()));
}

Task<> helloWorld() {
    co_await after(100_ms);
    co_return Ok();
}

test$(asyncTaskSleep) {
    return try$(runSync(helloWorld()));
}

Task<> taskA() {
    for (int i = 0; i < 10; ++i)
        co_await after(10_ms);
    co_return Ok();
}

Task<> taskB() {
    for (int i = 0; i < 10; ++i)
        co_await after(10_ms);
    co_return Ok();
}

test$(asyncTaskYield) {
    return try$(runSync(taskA()));
}

test$(asyncTestAll) {
    auto tA = taskA();
    auto tB = taskB();
    try$(runSync(Sys::waitAll(tA, tB)));
    return Ok();
}

Task<> taskInner() {
    co_await after(100_ms);
    co_return Ok();
}

Task<> taskOuter() {
    co_try_await$(taskInner());
    co_return Ok();
}

test$(asyncTaskNested) {
    return try$(runSync(taskOuter()));
}

} // namespace Karm::Sys::Tests
