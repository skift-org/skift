#include "radio.h"

namespace Karm::Kira {

struct Radio : public Ui::View<Radio> {
    bool _value = false;
    Ui::OnChange<bool> _onChange;
    Ui::MouseListener _mouseListener;

    Radio(bool value, Ui::OnChange<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Radio &o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Ui::ACCENT600 : Ui::ACCENT700);
            g.fill(bound(), 999);

            g.fillStyle(Gfx::GRAY50);
            g.fill(bound().shrink(6), 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::ACCENT600).withWidth(1).withAlign(Gfx::INSIDE_ALIGN));
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
        g.restore();
    }

    void event(App::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {26, 26};
    }
};

Ui::Child radio(bool value, Ui::OnChange<bool> onChange) {
    return makeStrong<Radio>(value, std::move(onChange));
}

} // namespace Karm::Kira
