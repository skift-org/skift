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

Res<> Driver::runAll() {
    usize passed = 0, failed = 0;

    Sys::errln("Running {} tests...\n", _tests.len());

    for (auto *test : _tests) {
        Sys::err(
            "Running {}: {}... ",
            test->_loc.file,
            Io::toNoCase(test->_name).unwrap()
        );

        auto result = test->run(*this);

        if (not result) {
            failed++;
            Sys::errln("{}", Cli::styled(result.none(), Cli::style(Cli::RED).bold()));
        } else {
            Sys::errln("{}", Cli::styled("PASS", Cli::style(Cli::GREEN).bold()));
            passed++;
        }
    }

    Sys::errln("");

    if (failed) {
        Sys::errln(
            " ❌ {} failled - {}",
            Cli::styled(failed, RED),
            Cli::styled(witty(Sys::now().val()), NOTE)
        );
        Sys::errln(
            "    {} passed\n",
            Cli::styled(passed, GREEN)
        );

        return Error::other("test failed");
    }

    Sys::errln(
        " {} {} passed - {}\n",
        goodEmoji(Sys::now().val()),
        Cli::styled(passed, GREEN),
        Cli::styled(nice(Sys::now().val()), NOTE)
    );

    return Ok();
}

Driver &driver() {
    static Opt<Driver> driver;
    if (not driver) {
        driver = Driver();
    }
    return *driver;
}

} // namespace Karm::Test
