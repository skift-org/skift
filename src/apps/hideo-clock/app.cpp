#include <hideo-base/scafold.h>
#include <karm-kira/navbar.h>
#include <karm-kira/toggle.h>
#include <karm-sys/time.h>
#include <karm-ui/anim.h>

#include "model.h"

namespace Hideo::Clock {

struct Clock : public Ui::View<Clock> {
    Time _time;

    Clock(Time time) : _time(time) {}

    void reconcile(Clock &o) override {
        _time = o._time;
    }

    void _drawHand(Gfx::Context &g, f64 angle, f64 length, Gfx::Color color, f64 width) {
        g.save();
        g.begin();
        g.translate(bound().center().cast<f64>());
        g.rotate(angle);
        g.line({0, {0, -length}});
        g.stroke(Gfx::stroke(color).withWidth(width).withCap(Gfx::ROUND_CAP).withAlign(Gfx::CENTER_ALIGN));
        g.restore();
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        auto size = bound().size().min();

        g.save();

        g.fillStyle(Ui::GRAY800);
        g.fill(Math::Ellipsef{bound().center().cast<f64>(), size / 2.});

        _drawHand(g, -_time.hour / 12.0 * 2 * M_PI, size / 2 * 0.5, Ui::GRAY500, size / 32.);
        _drawHand(g, -_time.minute / 60.0 * 2 * M_PI, size / 2 * 0.8, Ui::GRAY500, size / 32.);
        _drawHand(g, -_time.second / 60.0 * 2 * M_PI, size / 2 * 0.9, Ui::ACCENT500, size / 64.);

        g.fillStyle(Ui::GRAY600);
        g.fill(Math::Ellipsef{bound().center().cast<f64>(), size / 32.});

        g.restore();
    }
};

Ui::Child clock(Time time) {
    return makeStrong<Clock>(time);
}

// MARK: Alarm Page ------------------------------------------------------------

Ui::Child alarmCard(Time alarm, bool enabled) {
    return Ui::hflow(
               24,
               Math::Align::CENTER,
               Ui::displayMedium("{02}:{02}", alarm.hour, alarm.minute),
               Ui::grow(NONE),
               Kr::toggle(enabled, NONE)
           ) |
           Ui::box({
               .padding = 12,
               .borderRadii = 12,
               .backgroundPaint = Ui::GRAY900,
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
           Ui::spacing(12);
}

// MARK: Clock Page ------------------------------------------------------------

Ui::Child clockPage(State const &s) {
    auto time = s.dateTime.time;

    return Ui::vflow(
               12,
               clock(time) | Ui::pinSize({200, 200}),
               Ui::displayMedium("{02}:{02}:{02}", time.hour, time.minute, time.second) | Ui::center()
           ) |
           Ui::spacing(12);
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

Ui::Child appContent(State const &s) {
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

Ui::Child app() {
    return Ui::reducer<Model>(
        [](State const &s) {
            return scafold({
                .icon = Mdi::CLOCK,
                .title = "Clock"s,
                .titlebar = TitlebarStyle::DIALOG,
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
                            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::DOTS_HORIZONTAL)
                        ) | Ui::spacing({18, 18, 18, 0}),
                        appContent(s) | Ui::grow(),
                        navbar
                    );
                },
            });
        }
    );
}

Async::Task<> timerTask(Ui::Child app, Async::Cancelation::Token ct) {
    while (not ct.canceled()) {
        Model::event<TimeTick>(*app);
        co_trya$(Sys::globalSched().sleepAsync(Sys::now() + TimeSpan::fromSecs(1)));
    }
    co_return Ok();
}

} // namespace Hideo::Clock
