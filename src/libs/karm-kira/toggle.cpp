#include <karm-ui/anim.h>
#include <karm-ui/input.h>

namespace Karm::Kira {

struct Toggle : public Ui::View<Toggle> {
    static constexpr auto PADDING = 3;
    static constexpr auto SIZE = 18;

    bool _value = false;
    Ui::Easedf _anim;
    Ui::OnChange<bool> _onChange;
    Ui::MouseListener _mouseListener;

    Toggle(bool value, Ui::OnChange<bool> onChange)
        : _value(value), _onChange(std::move(onChange)) {
        _anim.set(*this, _value ? 1.0 : 0.0);
    }

    void reconcile(Toggle& o) override {
        if (_value != o._value) {
            _anim.animate(*this, _value ? 1.0 : 0.0, 0.1);
        }
        _value = o._value;
        _onChange = std::move(o._onChange);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();

        Math::Recti thumb = {
            bound().x + (isize)(PADDING + SIZE * _anim.value()),
            bound().y + PADDING,
            SIZE - PADDING * 2,
            SIZE - PADDING * 2,
        };

        auto inactiveBackground = (_mouseListener.isHover() ? Ui::GRAY600 : Ui::GRAY700);
        auto activeBackground = (_mouseListener.isHover() ? Ui::ACCENT600 : Ui::ACCENT700);

        g.fillStyle(inactiveBackground.lerpWith(activeBackground, _anim.value()));
        g.fill(bound(), 999);

        auto inactiveForeground = (_mouseListener.isHover() ? Ui::GRAY400 : Ui::GRAY500);
        auto activeForeground = Gfx::WHITE;

        g.fillStyle(inactiveForeground.lerpWith(activeForeground, _anim.value()));
        g.fill(thumb, 999);

        if (_mouseListener.isPress()) {
            auto inactivePressed = Ui::GRAY600;
            auto activePressed = Ui::ACCENT600;

            g.strokeStyle(
                Gfx::stroke(_value ? activePressed : inactivePressed)
                    .withWidth(1)
                    .withAlign(Gfx::INSIDE_ALIGN)
            );
            g.stroke(bound().cast<f64>(), 999);
        }

        g.pop();
    }

    void event(App::Event& e) override {
        if (_anim.needRepaint(*this, e))
            Ui::shouldRepaint(*this);

        if (_mouseListener.listen(*this, e)) {
            _value = not _value;
            _anim.animate(*this, _value ? 1.0 : 0.0, 0.1);
            _onChange(*this, _value);
        }
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {SIZE * 2, SIZE};
    }
};

Ui::Child toggle(bool value, Ui::OnChange<bool> onChange) {
    return makeRc<Toggle>(value, std::move(onChange));
}

} // namespace Karm::Kira
