module;

#include <karm-gfx/icon.h>
#include <karm-math/align.h>

export module Karm.Kira:slider;

import Karm.Ui;
import Karm.App;

namespace Karm::Kira {

export struct Slider : Ui::View<Slider> {
    static constexpr auto THUMP_RADIUS = 10;

    double _value = 0.0f;
    Opt<Ui::Send<double>> _onChange;
    Ui::MouseListener _mouseListener;

    Slider(double value, Opt<Ui::Send<double>> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Slider& o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();

        double v = (bound().width - THUMP_RADIUS * 2) * _value;
        auto thumbCenter = bound().startCenter().cast<double>() + Math::Vec2f{v + THUMP_RADIUS, 0};

        g.strokeStyle(Gfx::stroke(Ui::GRAY600).withWidth(4).withAlign(Gfx::CENTER_ALIGN).withCap(Gfx::ROUND_CAP));
        g.beginPath();
        g.moveTo(bound().startCenter().cast<double>() + Math::Vec2f{THUMP_RADIUS, 0});
        g.lineTo(bound().endCenter().cast<double>() - Math::Vec2f{THUMP_RADIUS, 0});
        g.stroke();

        g.strokeStyle(Gfx::stroke(_mouseListener.isHover() ? Ui::ACCENT400 : Ui::ACCENT500).withWidth(4).withAlign(Gfx::CENTER_ALIGN).withCap(Gfx::ROUND_CAP));
        g.beginPath();
        g.moveTo(bound().startCenter().cast<double>() + Math::Vec2f{THUMP_RADIUS, 0});
        g.lineTo(bound().startCenter().cast<double>() + Math::Vec2f{THUMP_RADIUS + v, 0});
        g.stroke();

        g.fillStyle(Ui::GRAY900);
        g.fill(Math::Ellipsef{thumbCenter, THUMP_RADIUS});

        g.strokeStyle(Gfx::stroke(Ui::GRAY700).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
        g.stroke();

        g.fillStyle(_mouseListener.isHover() ? Ui::ACCENT400 : Ui::ACCENT500);
        g.fill(Math::Ellipsef{thumbCenter, 6});

        g.pop();
    }

    void event(App::Event& e) override {
        _mouseListener.listen(*this, e);

        if (_mouseListener.isPress() and e.is<App::MouseEvent>()) {
            auto p = _mouseListener.pos();
            _value = (p.x - THUMP_RADIUS) / (static_cast<double>(bound().width) - THUMP_RADIUS * 2);
            _value = clamp01(_value);
            if (_onChange) {
                _onChange(*this, _value);
            }
            Ui::shouldRepaint(*this);
        }
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {128, 26};
    }
};

export Ui::Child slider(double value, Opt<Ui::Send<double>> onChange) {
    return makeRc<Slider>(value, std::move(onChange));
}

export Ui::Child slider(f64 value, Ui::Send<f64> onChange, Gfx::Icon icon, Str text) {
    return Ui::hflow(
               0,
               Math::Align::CENTER,
               Ui::icon(icon) |
                   Ui::center() |
                   Ui::aspectRatio(1) |
                   Ui::bound(),
               Ui::labelMedium(text)
           ) |
           Ui::box({
               .borderRadii = 6,
               .backgroundFill = Ui::ACCENT600,
           }) |
           Ui::dragRegion() |
           Ui::slider(value, std::move(onChange)) |
           Ui::box({
               .borderRadii = 6,
               .backgroundFill = Ui::GRAY900,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 36});
}

export template <typename T>
Ui::Child slider(T value, Range<T> range, Ui::Send<T> onChange, Gfx::Icon icon, Str text) {
    return slider(
        (value - range.start) / (f64)(range.end() - range.start),
        [=](Ui::Node& n, f64 v) {
            onChange(n, range.start + v * (range.end() - range.start));
        },
        icon,
        text
    );
}

} // namespace Karm::Kira
