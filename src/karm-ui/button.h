#pragma once

#include "funcs.h"
#include "proxy.h"
#include "text.h"

namespace Karm::Ui {

struct Button : public Proxy<Button> {
    enum State {
        IDLE,
        OVER,
        PRESS,
    };

    Func<void()> _onPress;
    State _state = IDLE;

    Button(Func<void()> onPress, Child child)
        : Proxy(child), _onPress(std::move(onPress)) {}

    void paint(Gfx::Context &g) override {
        g.fillStyle(Gfx::BLUE500);
        g.fill(bound());

        if (_state == OVER) {
            g.fillStyle(Gfx::BLACK.withOpacity(0.1));
            g.fill(bound());
        }

        if (_state == PRESS) {
            g.fillStyle(Gfx::BLACK.withOpacity(0.2));
            g.fill(bound());
        }

        child().paint(g);
    }

    void event(Events::Event &e) override {
        State state = _state;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (bound().contains(m.pos)) {
                if (state != PRESS)
                    state = OVER;

                if (m.type == Events::MouseEvent::PRESS) {
                    state = PRESS;
                    return true;
                }

                if (m.type == Events::MouseEvent::RELEASE) {
                    state = OVER;
                    _onPress();
                    return true;
                }
            } else {
                state = IDLE;
            }
            return false;
        });

        if (state != _state) {
            _state = state;
            Ui::shouldRepaint(*this);
        }
    };
};

Child button(Func<void()> onPress, Child child) {
    return makeStrong<Button>(std::move(onPress), child);
}

Child button(Func<void()> onPress, Str t) {
    return button(std::move(onPress), text(t));
}

} // namespace Karm::Ui
