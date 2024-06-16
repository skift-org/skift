#include <karm-ui/input.h>

namespace Karm::Kira {

struct Toggle : public Ui::View<Toggle> {
    bool _value = false;
    Ui::OnChange<bool> _onChange;
    Ui::MouseListener _mouseListener;

    Toggle(bool value, Ui::OnChange<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
    }

    void reconcile(Toggle &o) override {
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();

        auto thumb =
            bound()
                .hsplit(26)
                .get(_value)
                .shrink(_value ? 4 : 6);

        if (_value) {
            g.fillStyle(_mouseListener.isHover() ? Ui::ACCENT600 : Ui::ACCENT700);
            g.fill(bound(), 999);

            g.fillStyle(Ui::GRAY50);
            g.fill(thumb, 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::ACCENT600)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        } else {
            g.fillStyle(_mouseListener.isHover() ? Ui::GRAY600 : Ui::GRAY700);
            g.fill(bound(), 999);

            g.fillStyle(_mouseListener.isHover() ? Ui::GRAY400 : Ui::GRAY500);
            g.fill(thumb, 999);

            if (_mouseListener.isPress()) {
                g.strokeStyle(Gfx::stroke(Ui::GRAY600)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), 999);
            }
        }

        g.restore();
    }

    void event(Sys::Event &e) override {
        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {52, 26};
    }
};

Ui::Child toggle(bool value, Ui::OnChange<bool> onChange) {
    return makeStrong<Toggle>(value, std::move(onChange));
}

} // namespace Karm::Kira
