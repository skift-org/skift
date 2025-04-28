module;

#include <karm-base/time.h>
#include <karm-gfx/canvas.h>

export module Karm.Kira:clock;

import Karm.Ui;

namespace Karm::Kira {

struct Clock : Ui::View<Clock> {
    Time _time;

    Clock(Time time) : _time(time) {}

    void reconcile(Clock& o) override {
        _time = o._time;
    }

    void _drawHand(Gfx::Canvas& g, f64 angle, f64 length, Gfx::Color color, f64 width) {
        g.push();
        g.beginPath();
        g.translate(bound().center().cast<f64>());
        g.rotate(angle);
        g.line({0, {0, -length}});
        g.stroke(Gfx::stroke(color).withWidth(width).withCap(Gfx::ROUND_CAP).withAlign(Gfx::CENTER_ALIGN));
        g.pop();
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        auto size = bound().size().min();

        g.push();

        g.fillStyle(Ui::GRAY800);
        g.fill(Math::Ellipsef{bound().center().cast<f64>(), size / 2.});

        _drawHand(g, -_time.hour / 12.0 * 2 * Math::PI, size / 2 * 0.5, Ui::GRAY500, size / 32.);
        _drawHand(g, -_time.minute / 60.0 * 2 * Math::PI, size / 2 * 0.8, Ui::GRAY500, size / 32.);
        _drawHand(g, -_time.second / 60.0 * 2 * Math::PI, size / 2 * 0.9, Ui::ACCENT500, size / 64.);

        g.fillStyle(Ui::GRAY600);
        g.fill(Math::Ellipsef{bound().center().cast<f64>(), size / 32.});

        g.pop();
    }
};

export Ui::Child clock(Time time) {
    return makeRc<Clock>(time);
}

} // namespace Karm::Kira
