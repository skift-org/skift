#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Clock {

struct Clock : public Ui::View<Clock> {
    Time _time;

    Clock(Time time) : _time(time) {}

    void reconcile(Clock &o) override {
        _time = o._time;
    }

    void drawHand(Gfx::Context &g, f64 angle, f64 length, Gfx::Color color, f64 width) {
        g.save();
        g.begin();
        g.rotate(angle);
        g.translate(bound().center().cast<f64>());
        g.line({0, {0, -length}});
        g.stroke(Gfx::stroke(color).withWidth(width).withCap(Gfx::ROUND_CAP).withAlign(Gfx::CENTER_ALIGN));
        g.restore();
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        auto size = bound().size().min();

        g.save();

        g.fillStyle(Ui::GRAY800);
        g.fill(Math::Ellipsei{bound().center(), size / 2});

        drawHand(g, -_time.hour / 12.0 * 2 * M_PI, size / 2 * 0.5, Ui::GRAY500, size / 32.);
        drawHand(g, -_time.minute / 60.0 * 2 * M_PI, size / 2 * 0.8, Ui::GRAY500, size / 32.);
        drawHand(g, -_time.second / 60.0 * 2 * M_PI, size / 2 * 0.9, Ui::ACCENT500, size / 64.);

        g.fillStyle(Ui::GRAY600);
        g.fill(Math::Ellipsei{bound().center(), size / 32});

        g.restore();
    }
};

Ui::Child
navButton(bool selected, Ui::OnPress onPress, Mdi::Icon icon, char const *text) {
    return Ui::button(
        std::move(onPress),
        selected ? Ui::ButtonStyle::regular() : Ui::ButtonStyle::subtle(),
        Ui::vflow(
            6,
            Layout::Align::CENTER,
            Ui::icon(icon),
            Ui::labelMedium(text)) |
            Ui::spacing(8));
}

Ui::Child page(State const &s) {
    switch (s.page) {
    case Page::CLOCK:
        return makeStrong<Clock>(Sys::dateTime().time);
    case Page::STOPWATCH:
        return Ui::labelLarge("Stopwatch");
    case Page::TIMER:
        return Ui::labelLarge("Timer");
    case Page::ALARM:
        return Ui::labelLarge("Alarm");
    default:
        return Ui::labelLarge("Unknown");
    }
}

Ui::Child app() {
    return Ui::scafold({
        .icon = Mdi::CLOCK,
        .title = "Clock",
        .titlebar = Ui::TitlebarStyle::DIALOG,
        .body = Ui::reducer<Model>(
            [](State const &s) {
                return Ui::vflow(
                    Ui::vflow(
                        Ui::titleLarge(toStr(s.page)) | Ui::hcenter(), page(s) | Ui::grow()) |
                        Ui::spacing({8, 24, 8, 0}) | Ui::grow(),
                    Ui::separator(),
                    Ui::hflow(
                        4,
                        navButton(s.page == Page::ALARM, Model::bind(Page::ALARM), Mdi::ALARM, "Alarm") | Ui::grow(1),
                        navButton(s.page == Page::CLOCK, Model::bind(Page::CLOCK), Mdi::CLOCK_OUTLINE, "Clock") | Ui::grow(1),
                        navButton(s.page == Page::TIMER, Model::bind(Page::TIMER), Mdi::TIMER_SAND, "Timer") | Ui::grow(1),
                        navButton(s.page == Page::STOPWATCH, Model::bind(Page::STOPWATCH), Mdi::TIMER_OUTLINE, "Stopwatch") | Ui::grow(1)) |
                        Ui::box({
                            .padding = 8,
                            .backgroundPaint = Ui::GRAY900,
                        }));
            }),
    });
}

} // namespace Clock

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Clock::app());
}
