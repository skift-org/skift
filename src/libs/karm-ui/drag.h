#pragma once

#include "funcs.h"
#include "proxy.h"

namespace Karm::Ui {

struct Dragable {
    virtual ~Dragable() = default;

    virtual void drag(Math::Vec2i delta) = 0;
};

struct DragRegion : public Proxy<DragRegion> {
    bool _grabbed{};

    DragRegion(Child child) : Proxy(child) {}

    void event(Events::Event &e) override {
        if (!_grabbed)
            _child->event(e);

        if (e.accepted) {
            return;
        }

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (!bound().contains(m.pos) && !_grabbed) {
                return false;
            }

            if (m.type == Events::MouseEvent::PRESS) {
                _grabbed = true;
                return true;
            } else if (m.type == Events::MouseEvent::RELEASE) {
                _grabbed = false;
                return true;
            } else if (m.type == Events::MouseEvent::MOVE) {
                if (_grabbed) {
                    auto &p = queryParent<Dragable>(*this);
                    p.drag(m.delta);
                    return true;
                }
            }

            return false;
        });
    }
};

inline Child dragRegion(Child child) {
    return makeStrong<DragRegion>(child);
}

} // namespace Karm::Ui
