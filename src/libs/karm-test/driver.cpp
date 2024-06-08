#include <karm-base/witty.h>
#include <karm-cli/cursor.h>
#include <karm-cli/spinner.h>
#include <karm-cli/style.h>
#include <karm-sys/chan.h>
#include <karm-sys/time.h>

#include "driver.h"
#include "test.h"

namespace Karm::Test {

namespace {

constexpr auto GREEN = Cli::Style{Cli::GREEN}.bold();
constexpr auto RED = Cli::Style{Cli::RED}.bold();
constexpr auto NOTE = Cli::Style{Cli::GRAY_DARK}.bold();

} // namespace

void Driver::add(Test *test) {
    _tests.pushBack(test);
}

Async::Task<> Driver::runAllAsync() {
    usize passed = 0, failed = 0;

    Sys::errln("Running {} tests...\n", _tests.len());

    for (auto *test : _tests) {
        Sys::err(
            "Running {}: {}... ",
            test->_loc.file,
            Io::toNoCase(test->_name).unwrap()
        );

        auto result = co_await test->runAsync(*this);

        if (not result) {
            failed++;
            Sys::errln("{}", Cli::styled(result.none(), Cli::style(Cli::RED).bold()));
        } else {
            passed++;
            Sys::errln("{}", Cli::styled("PASS"s, Cli::style(Cli::GREEN).bold()));
        }
    }

    Sys::errln("");

    if (failed) {
        Sys::errln(
            " {5} failled - {} {}",
            Cli::styled(failed, RED),
            Cli::styled(witty(Sys::now().val()), NOTE),
            badEmoji(Sys::now().val())
        );
        Sys::errln(
            " {5} passed\n",
            Cli::styled(passed, GREEN)
        );

        co_return Error::other("test failed");
    }

    Sys::errln(
        "{5} passed - {} {}\n",
        Cli::styled(passed, GREEN),
        Cli::styled(nice(Sys::now().val()), NOTE),
        goodEmoji(Sys::now().val())
    );

    co_return Ok();
}

Driver &driver() {
    static Opt<Driver> driver;
    if (not driver) {
        driver = Driver();
    }
    return *driver;
}

} // namespace Karm::Test
