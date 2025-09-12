#include <karm-sys/chan.h>
#include <karm-sys/time.h>

#include "driver.h"
#include "test.h"

import Karm.Core;
import Karm.Cli;

namespace Karm::Test {

namespace {

constexpr auto GREEN = Tty::Style{Tty::GREEN}.bold();
constexpr auto RED = Tty::Style{Tty::RED}.bold();
constexpr auto YELLOW = Tty::Style{Tty::YELLOW}.bold();
constexpr auto NOTE = Tty::Style{Tty::GRAY_DARK}.bold();

} // namespace

void Driver::add(Test* test) {
    _tests.pushBack(test);
}

Async::Task<> Driver::runAllAsync(RunOptions options) {
    usize passed = 0, failed = 0, skipped = 0;

    Sys::errln("Running {} tests...", _tests.len());
    if (options.glob != "*")
        Sys::errln("Matching glob: {#}", options.glob);

    Sys::errln("");

    for (auto* test : _tests) {
        if (not Glob::matchGlob(options.glob, test->_name))
            continue;

        Sys::err(
            "Running {}: {}... ",
            test->_loc.file,
            Io::toNoCase(test->_name).unwrap()
        );

        auto result = co_await test->runAsync(*this);

        if (not result and result.none() == Error::SKIPPED) {
            skipped++;
            Sys::errln("{}", "SKIP"s | Tty::style(Tty::YELLOW).bold());
        } else if (not result) {
            if (options.fast) {
                Sys::errln("{}", "FAIL"s | Tty::style(Tty::RED).bold());
                co_return Error::other("test failed");
            }
            failed++;
            Sys::errln("{}", Io::cased(result, Io::Case::UPPER) | Tty::style(Tty::RED).bold());
        } else {
            passed++;
            Sys::errln("{}", "PASS"s | Tty::style(Tty::GREEN).bold());
        }
    }

    Sys::errln("");

    if (skipped) {
        Sys::errln(
            " {5} skipped",
            skipped | YELLOW
        );
    }

    if (failed) {
        Sys::errln(
            " {5} failed - {} {}",
            failed | RED,
            witty(Sys::now().val()) | NOTE,
            badEmoji(Sys::now().val())
        );
        Sys::errln(
            " {5} passed\n",
            passed | GREEN
        );

        co_return Error::other("test failed");
    }

    Sys::errln(
        " {5} passed - {} {}\n",
        passed | GREEN,
        nice(Sys::now().val()) | NOTE,
        goodEmoji(Sys::now().val())
    );

    co_return Ok();
}

Driver& driver() {
    static Opt<Driver> driver;
    if (not driver) {
        driver = Driver();
    }
    return *driver;
}

} // namespace Karm::Test
