#include <karm-logger/logger.h>
#include <karm-sys/async.h>
#include <karm-sys/proc.h>
#include <karm-test/macros.h>

namespace Karm::Sys::Tests {

Async::Task<> sleepyBoy() {
    auto duration = TimeSpan::fromMSecs(100);
    auto start = Sys::now();
    co_trya$(globalSched().sleepAsync(start + duration));
    auto end = Sys::now();

    if (end - start < duration - TimeSpan::fromMSecs(10))
        co_return Error::other("sleepAsync woke up too early");

    if (end - start > duration + TimeSpan::fromMSecs(10))
        co_return Error::other("sleepAsync woke up too late");

    co_return Ok();
}

test$(sleepAsync) {
    return Sys::run(sleepyBoy());
}

} // namespace Karm::Sys::Tests
