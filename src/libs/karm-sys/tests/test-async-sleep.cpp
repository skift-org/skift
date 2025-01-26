#include <karm-logger/logger.h>
#include <karm-sys/async.h>
#include <karm-sys/proc.h>
#include <karm-test/macros.h>

namespace Karm::Sys::Tests {

Async::Task<> sleepyBoy() {
#ifdef __ck_sys_darwin__
    logInfo("Skipping test on macOS");
    co_return Error::skipped();
#endif

    auto duration = Duration::fromMSecs(100);
    auto start = Sys::instant();
    co_trya$(globalSched().sleepAsync(start + duration));
    auto end = Sys::instant();

    logDebug("slept for {}ms", (end - start).toMSecs());

    if (end - start < duration - Duration::fromMSecs(10))
        co_return Error::other("sleepAsync woke up too early");

    if (end - start > duration + Duration::fromMSecs(10))
        co_return Error::other("sleepAsync woke up too late");

    co_return Ok();
}

testAsync$("async-sleep") {
    return sleepyBoy();
}

} // namespace Karm::Sys::Tests
