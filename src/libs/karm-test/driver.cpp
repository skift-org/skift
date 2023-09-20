#include <karm-base/witty.h>
#include <karm-cli/cursor.h>
#include <karm-cli/spinner.h>
#include <karm-cli/style.h>
#include <karm-fmt/case.h>
#include <karm-sys/chan.h>
#include <karm-sys/time.h>

#include "driver.h"
#include "test.h"

namespace Karm::Test {

void Driver::add(Test *test) {
    _tests.pushBack(test);
}

static auto GREEN = Cli::Style{Cli::GREEN}.bold();
static auto RED = Cli::Style{Cli::RED}.bold();
static auto NOTE = Cli::Style{Cli::GRAY_DARK}.bold();

void Driver::runAll() {
    usize passed = 0, failed = 0;

    Sys::errln("Running {} tests...\n", _tests.len());

    for (auto *test : _tests) {
        Sys::err("{}{} Running {}...{}{}", Cli::Cmd::clearLineAfter(), Cli::styled(" TEST ", Cli::style().bold().bg(Cli::CYAN)), Fmt::toNoCase(test->_name).unwrap(), Cli::Cmd::horizontal(0));

        auto result = test->run(*this);
        auto label = result
                         ? Cli::styled(" PASS ", Cli::style(Cli::WHITE).bold().bg(Cli::GREEN_LIGHT))
                         : Cli::styled(" FAIL ", RED);

        Sys::err("{}{} {}", Cli::Cmd::clearLineAfter(), label, Fmt::toNoCase(test->_name).unwrap());

        if (not result) {
            Sys::errln(" - {}", Cli::styled(result.none().msg(), RED));
            failed++;
        } else {
            Sys::errln("");
            passed++;
        }
    }

    Sys::errln("");

    if (failed) {
        Sys::errln(" ❌ {} failled - {}",
                   Cli::styled(failed, RED),
                   Cli::styled(witty(Sys::now().val()), NOTE));
        Sys::errln("    {} passed",
                   Cli::styled(passed, GREEN));
    } else {
        Sys::errln(" 🤘 {} passed - {}",
                   Cli::styled(passed, GREEN),
                   Cli::styled(nice(Sys::now().val()), NOTE));
    }
}

Driver &driver() {
    static Opt<Driver> driver;
    if (not driver) {
        driver = Driver();
    }
    return *driver;
}

} // namespace Karm::Test
