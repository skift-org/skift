module;

#include <karm-async/cancelation.h>
#include <karm-math/align.h>
#include <karm-sys/time.h>

export module Hideo.Clock:app;

import Mdi;
import Karm.Kira;
import Karm.Ui;
import :model;

namespace Hideo::Clock {

// MARK: Alarm Page ------------------------------------------------------------

Ui::Child alarmCard(Time alarm, bool enabled) {
    return Ui::hflow(
               24,
               Math::Align::CENTER,
               Ui::displayMedium("{02}:{02}", alarm.hour, alarm.minute),
               Ui::grow(NONE),
               Kr::toggle(enabled, Ui::SINK<bool>)
           ) |
           Ui::box({
               .padding = 12,
               .borderRadii = 12,
               .backgroundFill = Ui::GRAY900,
           });
}

Ui::Child alarmPage() {
    return Ui::vflow(
               12,
               alarmCard({0, 0, 8}, true),
               alarmCard({0, 0, 12}, false),
               alarmCard({0, 0, 18}, true),
               alarmCard({0, 0, 22}, false)
           ) |
           Ui::insets(12);
}

// MARK: Clock Page ------------------------------------------------------------

Ui::Child clockPage(State const& s) {
    auto time = s.dateTime.time;

    return Ui::vflow(
               12,
               Kr::clock(time) | Ui::pinSize({200, 200}),
               Ui::displayMedium("{02}:{02}:{02}", time.hour, time.minute, time.second) | Ui::center()
           ) |
           Ui::insets(12);
}

// MARK: Timer Page ------------------------------------------------------------

Ui::Child timerPage() {
    return Ui::labelLarge("Timer");
}

// MARK: Stopwatch Page --------------------------------------------------------

Ui::Child stopwatchPage() {
    return Ui::labelLarge("Stopwatch");
}

// MARK: App -------------------------------------------------------------------

Ui::Child appContent(State const& s) {
    switch (s.page) {
    case Page::ALARM:
        return alarmPage();
    case Page::CLOCK:
        return clockPage(s);
    case Page::TIMER:
        return timerPage();
    case Page::STOPWATCH:
        return stopwatchPage();
    }
}

export Ui::Child app() {
    return Ui::reducer<Model>(
        [](State const& s) {
            return Kr::scaffold({
                .icon = Mdi::CLOCK,
                .title = "Clock"s,
                .titlebar = Kr::TitlebarStyle::DIALOG,
                .body = [&] {
                    auto navbar = Kr::navbarContent({
                        Kr::navbarItem(
                            Model::bind(Page::ALARM),
                            Mdi::ALARM,
                            "Alarm",
                            s.page == Page::ALARM
                        ),
                        Kr::navbarItem(
                            Model::bind(Page::CLOCK),
                            Mdi::CLOCK_OUTLINE,
                            "Clock",
                            s.page == Page::CLOCK
                        ),
                        Kr::navbarItem(
                            Model::bind(Page::TIMER),
                            Mdi::TIMER_SAND,
                            "Timer",
                            s.page == Page::TIMER
                        ),
                        Kr::navbarItem(
                            Model::bind(Page::STOPWATCH),
                            Mdi::TIMER_OUTLINE,
                            "Stopwatch",
                            s.page == Page::STOPWATCH
                        ),
                    });

                    return Ui::vflow(
                        Ui::hflow(
                            0,
                            Math::Align::CENTER,
                            Ui::titleLarge(toStr(s.page)),
                            Ui::grow(NONE),
                            Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::DOTS_HORIZONTAL)
                        ) | Ui::insets({18, 18, 0, 18}),
                        appContent(s) | Ui::grow(),
                        navbar
                    );
                },
            });
        }
    );
}

export Async::Task<> timerTask(Ui::Child app, Async::Ct ct) {
    while (not ct.canceled()) {
        Model::event<TimeTick>(*app);
        co_trya$(Sys::globalSched().sleepAsync(Sys::instant() + Duration::fromSecs(1)));
    }
    co_return Ok();
}

} // namespace Hideo::Clock
