#include <hideo-base/scafold.h>
#include <karm-kira/navbar.h>
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
        g.fill(Math::Ellipsei{bound().center(), size / 2});

        _drawHand(g, -_time.hour / 12.0 * 2 * M_PI, size / 2 * 0.5, Ui::GRAY500, size / 32.);
        _drawHand(g, -_time.minute / 60.0 * 2 * M_PI, size / 2 * 0.8, Ui::GRAY500, size / 32.);
        _drawHand(g, -_time.second / 60.0 * 2 * M_PI, size / 2 * 0.9, Ui::ACCENT500, size / 64.);

        g.fillStyle(Ui::GRAY600);
        g.fill(Math::Ellipsei{bound().center(), size / 32});

        g.restore();
    }
};

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

                    auto body = Ui::carousel(
                        (usize)s.page,
                        {
                            Ui::labelLarge("Alarm"),
                            makeStrong<Clock>(Sys::dateTime().time),
                            Ui::labelLarge("Timer"),
                            Ui::labelLarge("Stopwatch"),
                        }
                    );

                    return Ui::vflow(
                        Ui::vflow(
                            Ui::titleLarge(toStr(s.page)) | Ui::hcenter(),
                            body | Ui::grow()
                        ) |
                            Ui::spacing({8, 24, 8, 0}) | Ui::grow(),
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
