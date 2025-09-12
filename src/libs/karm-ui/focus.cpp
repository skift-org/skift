module;

#include <karm-gfx/canvas.h>

export module Karm.Ui:focus;

import Karm.App;
import :funcs;
import :atoms;
import :intent;

namespace Karm::Ui {

export struct FocusEvent {
    enum struct Type {
        STEAL,
        ENTER,
        LEAVE,
    };
    using enum Type;
    Type type;
};

export struct FocusListener {
    bool _focused = false;

    void event(Ui::Node& n, App::Event& e) {
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

struct Focusable : ProxyNode<Focusable> {
    bool _focused = false;

    Focusable(Ui::Child child)
        : ProxyNode<Focusable>(std::move(child)) {
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        ProxyNode<Focusable>::paint(g, r);

        if (_focused) {
            g.strokeStyle(Gfx::stroke(ACCENT500).withWidth(2).withAlign(Gfx::INSIDE_ALIGN));
            g.stroke(bound().cast<f64>(), 4);
        }
    }

    void _stealFocus() {
        bubble<FocusEvent>(*this, FocusEvent::STEAL);
        _focused = true;
        shouldRepaint(*this);
        event<FocusEvent>(*_child, FocusEvent::ENTER);
    }

    void event(App::Event& e) override {
        bool passthrough = false;

        if (auto fe = e.is<FocusEvent>()) {
            if (fe->type == FocusEvent::STEAL and _focused) {
                _focused = false;
                shouldRepaint(*this);
                event<FocusEvent>(*_child, FocusEvent::LEAVE);
            }
        } else if (auto me = e.is<App::MouseEvent>()) {
            passthrough = true;
            if (me->type == App::MouseEvent::PRESS) {
                if (bound().contains(me->pos) and not _focused) {
                    _stealFocus();
                } else if (not bound().contains(me->pos) and _focused) {
                    _focused = false;
                    shouldRepaint(*this);
                    event<FocusEvent>(*_child, FocusEvent::LEAVE);
                }
            }
        } else if (auto it = e.is<KeyboardShortcutActivated>()) {
            _stealFocus();
            e.accept();
        }

        if (_focused or passthrough)
            ProxyNode<Focusable>::event(e);
    }
};

export Ui::Child focusable(Ui::Child child) {
    return makeRc<Focusable>(std::move(child));
}

export auto focusable() {
    return [](Ui::Child child) {
        return focusable(std::move(child));
    };
}

} // namespace Karm::Ui
