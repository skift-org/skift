#pragma once

#include <karm-app/inputs.h>
#include <karm-ui/node.h>

#include "funcs.h"

namespace Karm::Ui {

struct FocusEvent {
    enum struct Type {
        STEAL,
        ENTER,
        LEAVE,
    };
    using enum Type;
    Type type;

    FocusEvent(Type type) : type(type) {
    }
};

struct FocusListener {
    bool _focused = false;

    void event(Ui::Node &n, App::Event &e) {
        if (auto fe = e.is<FocusEvent>()) {
            if (fe->type == FocusEvent::ENTER) {
                _focused = true;
                shouldRepaint(n);
            } else if (fe->type == FocusEvent::LEAVE) {
                _focused = false;
                shouldRepaint(n);
            }
        }
    }

    bool focused() const {
        return _focused;
    }

    operator bool() const {
        return _focused;
    }
};

struct Focusable : public ProxyNode<Focusable> {
    bool _focused = false;

    Focusable(Ui::Child child) : ProxyNode<Focusable>(std::move(child)) {
    }

    void paint(Gfx::Canvas &g, Math::Recti r) override {
        ProxyNode<Focusable>::paint(g, r);

        if (_focused) {
            g.strokeStyle(Gfx::stroke(ACCENT600).withWidth(2).withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound().cast<f64>(), 4);
        }
    }

    void event(App::Event &e) override {
        if (auto fe = e.is<FocusEvent>()) {
            if (fe->type == FocusEvent::STEAL and _focused) {
                _focused = false;
                shouldRepaint(*this);
                event<FocusEvent>(*_child, FocusEvent::LEAVE);
            }
        } else if (auto me = e.is<App::MouseEvent>()) {
            if (me->type == App::MouseEvent::PRESS) {
                if (bound().contains(me->pos) and not _focused) {
                    bubble<FocusEvent>(*this, FocusEvent::STEAL);
                    _focused = true;
                    shouldRepaint(*this);
                    event<FocusEvent>(*_child, FocusEvent::ENTER);
                } else if (not bound().contains(me->pos) and _focused) {
                    _focused = false;
                    shouldRepaint(*this);
                    event<FocusEvent>(*_child, FocusEvent::LEAVE);
                }
            }
        }

        if (_focused)
            ProxyNode<Focusable>::event(e);
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
