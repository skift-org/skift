#pragma once

#include <karm-app/inputs.h>
#include <karm-ui/node.h>

#include "funcs.h"

namespace Karm::Ui {

struct StealFocus {};

struct Focusable : public ProxyNode<Focusable> {
    bool _focused = false;

    Focusable(Ui::Child child) : ProxyNode<Focusable>(std::move(child)) {
    }

    void paint(Gfx::Canvas &g, Math::Recti r) override {
        ProxyNode<Focusable>::paint(g, r);

        if (_focused) {
            g.strokeStyle(Gfx::stroke(ACCENT600).withWidth(2));
            g.stroke(bound().cast<f64>());
        }
    }

    void event(App::Event &e) override {
        if (e.is<StealFocus>() and _focused) {
            _focused = false;
            shouldRepaint(*this);
        } else if (auto me = e.is<App::MouseEvent>()) {
            if (bound().contains(me->pos) and
                me->type == App::MouseEvent::PRESS and
                not _focused) {
                bubble<StealFocus>(*this);
                _focused = true;
                shouldRepaint(*this);
            }
        }

        if (not _focused) {
            ProxyNode<Focusable>::event(e);
        }
    }
};

static inline Ui::Child focusable(Ui::Child child) {
    return makeStrong<Focusable>(std::move(child));
}

static inline auto focusable() {
    return [](Ui::Child child) {
        return focusable(std::move(child));
    };
}

} // namespace Karm::Ui
