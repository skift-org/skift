#include <karm-image/loader.h>
#include <karm-math/align.h>
#include <karm-sys/entry.h>

import Karm.Ui;

namespace Hideo::Wear {

Ui::Child app() {
    auto [date, time] = DateTime{
        .date = {1, Month::APRIL, 2024},
        .time = {56, 34, 12},
    };

    auto dateTime = Io::format(
        // Mon. 28 Jul
        "{}. {} {}",
        Io::toCapitalCase(date.dayOfWeek().abbr()),
        date.dayOfMonth() + 1,
        Io::toCapitalCase(date.month.str())
    );

    auto clock = Ui::vflow(
        16,
        Math::Align::CENTER,
        Ui::displayMedium("{02}:{02}", time.hour, time.minute),
        Ui::titleMedium(dateTime)
    );

    return clock |
           Ui::center() |
           Ui::pinSize(192);
}

} // namespace Hideo::Wear

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(
        ctx,
        Hideo::Wear::app()
    );
}
