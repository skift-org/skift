module;

#include <karm-app/event.h>
#include <karm-gfx/canvas.h>
#include <karm-gfx/icon.h>

export module Karm.Kira:checkbox;

import Karm.Ui;
import Mdi;

namespace Karm::Kira {

struct Checkbox : Ui::View<Checkbox> {
    bool _value = false;
    Ui::Send<bool> _onChange;
    Ui::MouseListener _mouseListener;

    Checkbox(bool value, Ui::Send<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Checkbox& o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();

        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Ui::ACCENT600 : Ui::ACCENT700);
            g.fill(bound(), 4);

            g.fillStyle(Gfx::GRAY50);
            Gfx::Icon{Mdi::CHECK_BOLD}.fill(g, bound().topStart().cast<f64>(), 18);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::ACCENT600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound().cast<f64>(), 4);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? Ui::GRAY600 : Ui::GRAY700);
            g.fill(bound(), 4);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::GRAY600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound().cast<f64>(), 4);
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

export Ui::Child checkbox(bool value, Ui::Send<bool> onChange) {
    return makeRc<Checkbox>(value, std::move(onChange));
}

} // namespace Karm::Kira
