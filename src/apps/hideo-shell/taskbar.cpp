module;

#include <karm-gfx/filters.h>
#include <karm-io/fmt.h>
#include <karm-math/align.h>

export module Hideo.Shell:taskbar;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import Hideo.Keyboard;
import :model;

using namespace Karm;

namespace Hideo::Shell {

Ui::Child taskbarAppsButton() {
    return Ui::button(
        Model::bind<Activate>(Panel::APPS),
        Ui::ButtonStyle::subtle(),
        Mdi::APPS, "Applications"
    );
}

Ui::Child taskbarCalendarButton(State const& s) {
    auto [date, time] = s.dateTime;

    auto dateTime = Io::format(
        "{}. {} {}, {02}:{02}",
        Io::toCapitalCase(date.month.abbr()),
        date.dayOfMonth() + 1,
        date.year.val(),
        time.hour,
        time.minute
    );

    return Ui::button(
        Model::bind<Activate>(Panel::NOTIS),
        Ui::ButtonStyle::subtle(),
        Mdi::CALENDAR,

        dateTime
    );
}

Ui::Child taskbarStatusButton() {
    return Ui::button(
        Model::bind<Activate>(Panel::SYS),
        Ui::ButtonStyle::subtle(),
        Ui::hflow(
            6,
            Math::Align::CENTER,
            Ui::icon(Mdi::WIFI_STRENGTH_4),
            Ui::icon(Mdi::VOLUME_HIGH),
            Ui::icon(Mdi::BATTERY),
            Ui::labelMedium("100%")
        ) |

            Ui::center() |
            Ui::insets({6, 12}) |
            Ui::bound()
    );
}

Ui::Child taskbar(State const& s) {
    return Ui::vflow(
               Ui::hflow(
                   6,
                   taskbarAppsButton(),
                   taskbarCalendarButton(s) |
                       Ui::center() |
                       Ui::grow(),
                   Ui::button(
                       Keyboard::show,
                       Ui::ButtonStyle::subtle(),
                       Mdi::KEYBOARD
                   ),
                   taskbarStatusButton()
               ) |
                   Ui::box({
                       .padding = 6,
                       .backgroundFill = Ui::GRAY950.withOpacity(0.6),
                   }),
               Kr::separator()
           ) |
           Ui::backgroundFilter(Gfx::BlurFilter{16});
}

} // namespace Hideo::Shell
