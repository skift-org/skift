module;

#include <karm-gfx/canvas.h>

export module Karm.Kira:radio;

import Karm.App;
import Karm.Ui;

namespace Karm::Kira {

struct Radio : Ui::View<Radio> {
    bool _value = false;
    Ui::Send<bool> _onChange;
    Ui::MouseListener _mouseListener;

    Radio(bool value, Ui::Send<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Radio& o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Ui::ACCENT400 : Ui::ACCENT500);
            g.fill(bound(), 999);

            g.fillStyle(Gfx::GRAY50);
            g.fill(bound().shrink(4), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::ACCENT500).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound().cast<f64>(), 999);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? Ui::GRAY600 : Ui::GRAY700);
            g.fill(bound(), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::GRAY600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound().cast<f64>(), 999);
            }
        }
        g.pop();
    }

    void event(App::Event& e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {18, 18};
    }
};

export Ui::Child radio(bool value, Ui::Send<bool> onChange) {
    return makeRc<Radio>(value, std::move(onChange));
}

} // namespace Karm::Kira
