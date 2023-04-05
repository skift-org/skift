#include <karm-cli/style.h>
#include <karm-sys/chan.h>
#include <karm-sys/time.h>
#include <karm-text/witty.h>

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

    Sys::errln("Running {} tests...", _tests.len());

    Sys::errln("");

    for (auto *test : _tests) {
        auto result = test->run(*this);

        auto label = result
                         ? Cli::styled("PASS", GREEN)
                         : Cli::styled("FAIL", RED);

        Sys::err(" {} {}", label, test->_name);

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
                   Cli::styled(Text::witty(Sys::now().val()), NOTE));
        Sys::errln("    {} passed",
                   Cli::styled(passed, GREEN));
    } else {
        Sys::errln(" 🤘 {} passed - {}",
                   Cli::styled(passed, GREEN),
                   Cli::styled(Text::nice(Sys::now().val()), NOTE));
    }

    Sys::errln("");
}

Driver &driver() {
    static Opt<Driver> driver;
    if (not driver) {
        driver = Driver();
    }
    return *driver;
}

} // namespace Karm::Test
