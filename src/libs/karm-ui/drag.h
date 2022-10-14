#pragma once

#include "funcs.h"
#include "proxy.h"

namespace Karm::Ui {

/* --- Draggable ------------------------------------------------------------ */

struct Dragable {
    virtual ~Dragable() = default;

    virtual void drag(Math::Vec2i delta) = 0;
};

/* --- Dismisable ----------------------------------------------------------- */

enum struct DismisDir {
    LEFT = 1 << 0,
    RIGHT = 1 << 1,
    TOP = 1 << 2,
    DOWN = 1 << 3,

    HORIZONTAL = LEFT | RIGHT,
    VERTICAL = TOP | DOWN,
};

FlagsEnum$(DismisDir);

using OnDismis = Func<void(Node &)>;

struct Dismisable :
    public Proxy<Dismisable>,
    public Dragable {

    OnDismis _onDismis;
    DismisDir _dir;
    double _threshold;
    Math::Vec2i _drag{};
    bool _dismissed{};
    bool _animated{};

    Dismisable(OnDismis onDismis, DismisDir dir, double threshold, Ui::Child child)
        : Proxy(child), _onDismis(std::move(onDismis)), _dir(dir), _threshold(threshold) {}

    void drag(Math::Vec2i delta) override {
        auto d = _drag + delta;

        d.x = clamp(
            d.x,
            (bool)(_dir & DismisDir::LEFT) ? -bound().width : 0,
            (bool)(_dir & DismisDir::RIGHT) ? bound().width : 0);

        d.y = clamp(
            d.y,
            (bool)(_dir & DismisDir::TOP) ? -bound().height : 0,
            (bool)(_dir & DismisDir::DOWN) ? bound().height : 0);

        _drag = d;

        if (_dismissed)
            return;

        if ((bool)(_dir & DismisDir::HORIZONTAL)) {
            if (std::abs(_drag.x) / (double)bound().width > _threshold) {
                _dismissed = true;
                _onDismis(child());
            }
        }
        if ((bool)(_dir & DismisDir::VERTICAL)) {
            if (std::abs(_drag.y) / (double)bound().height > _threshold) {
                _dismissed = true;
                _onDismis(child());
            }
        }

        _animated = true;
        Ui::shouldAnimate(*this);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        g.clip(bound());
        g.origin(_drag);
        r.xy = r.xy - _drag;
        child().paint(g, r);

        g.restore();
    }

    void event(Events::Event &e) override {
        if (e.is<Events::AnimateEvent>() && _animated) {
            shouldRepaint(*parent(), bound());
            _animated = false;
        } else {
            Ui::Proxy<Dismisable>::event(e);
        }
    }

    void *query(Meta::Id id) override {
        if (id == Meta::makeId<Ui::Dragable>()) {
            return static_cast<Ui::Dragable *>(this);
        }
        return Ui::Proxy<Dismisable>::query(id);
    }
};

inline Child dismisable(OnDismis onDismis, DismisDir dir, double threshold, Ui::Child child) {
    return makeStrong<Dismisable>(std::move(onDismis), dir, threshold, std::move(child));
}

/* --- Drag Region ---------------------------------------------------------- */

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
