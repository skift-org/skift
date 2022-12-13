#pragma once

#include "anim.h"
#include "input.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

/* --- Drag Event ----------------------------------------------------------- */

struct DragEvent : public Events::_Event<DragEvent, 0x8614c312b36a0215> {
    enum _Type : uint8_t {
        START,
        DRAG,
        END
    };

    _Type type;
    Math::Vec2i delta;
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
    public ProxyNode<Dismisable> {

    OnDismis _onDismis;
    DismisDir _dir;
    double _threshold;
    Anim2<double> _drag{};
    bool _dismissed{};
    bool _animated{};

    Dismisable(OnDismis onDismis, DismisDir dir, double threshold, Ui::Child child)
        : ProxyNode(child), _onDismis(std::move(onDismis)), _dir(dir), _threshold(threshold) {}

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        auto dragi = _drag.value().cast<int>();

        g.clip(bound());
        g.origin(dragi);
        r.xy = r.xy - dragi;
        child().paint(g, r);

        g.restore();
    }

    void event(Events::Event &e) override {
        _drag.event(*this, e);
        if (_dismissed and _drag.reached()) {
            _onDismis(*this);
        }
        Ui::ProxyNode<Dismisable>::event(e);
    }

    void bubble(Events::Event &e) override {
        if (e.is<DragEvent>()) {
            auto &de = e.unwrap<DragEvent>();

            if (de.type == DragEvent::DRAG) {
                auto d = _drag.target() + de.delta;

                d.x = clamp(
                    d.x,
                    (bool)(_dir & DismisDir::LEFT) ? -bound().width : 0,
                    (bool)(_dir & DismisDir::RIGHT) ? bound().width : 0);

                d.y = clamp(
                    d.y,
                    (bool)(_dir & DismisDir::TOP) ? -bound().height : 0,
                    (bool)(_dir & DismisDir::DOWN) ? bound().height : 0);

                _drag.set(*this, d);
            } else if (de.type == DragEvent::END) {
                if ((bool)(_dir & DismisDir::HORIZONTAL)) {
                    if (Math::abs(_drag.targetX()) / (double)bound().width > _threshold) {
                        _drag.animate(*this, {bound().width * (_drag.targetX() < 0.0 ? -1.0 : 1), 0}, 0.25, Math::Easing::cubicOut);
                        _dismissed = true;
                    } else {
                        _drag.animate(*this, {0, _drag.targetY()}, 0.25, Math::Easing::exponentialOut);
                    }
                }
                if ((bool)(_dir & DismisDir::VERTICAL)) {
                    if (Math::abs(_drag.targetY()) / (double)bound().height > _threshold) {
                        _drag.animate(*this, {0, bound().height * (_drag.targetY() < 0.0 ? -1.0 : 1)}, 0.25, Math::Easing::cubicOut);
                        _dismissed = true;
                    } else {
                        _drag.animate(*this, {_drag.targetX(), 0}, 0.25, Math::Easing::exponentialOut);
                    }
                }
            }
        } else {
            Ui::ProxyNode<Dismisable>::bubble(e);
        }
    }
};

inline Child dismisable(OnDismis onDismis, DismisDir dir, double threshold, Ui::Child child) {
    return makeStrong<Dismisable>(std::move(onDismis), dir, threshold, std::move(child));
}

/* --- Drag Region ---------------------------------------------------------- */

struct DragRegion : public ProxyNode<DragRegion> {
    bool _grabbed{};

    DragRegion(Child child) : ProxyNode(child) {}

    void event(Events::Event &e) override {
        if (not _grabbed)
            _child->event(e);

        if (e.accepted) {
            return;
        }

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (not bound().contains(m.pos) and not _grabbed) {
                return false;
            }

            if (m.type == Events::MouseEvent::PRESS) {
                _grabbed = true;

                DragEvent de = {.type = DragEvent::START};
                bubble(de);

                return true;
            } else if (m.type == Events::MouseEvent::RELEASE) {
                _grabbed = false;

                DragEvent de = {.type = DragEvent::END};
                bubble(de);

                return true;
            } else if (m.type == Events::MouseEvent::MOVE) {
                if (_grabbed) {

                    DragEvent de = {.type = DragEvent::DRAG, .delta = m.delta};
                    bubble(de);

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

/* --- Handle --------------------------------------------------------------- */

inline Child handle() {
    return minSize(
        {
            UNCONSTRAINED,
            48,
        },
        center(
            spacing(
                12,
                box(
                    BoxStyle{
                        .borderRadius = 999,
                        .backgroundPaint = Gfx::WHITE,
                    },
                    empty({
                        128,
                        4,
                    })))));
}

inline Child dragHandle() {
    return dragRegion(
        handle());
}

inline Child buttonHandle(OnPress press) {
    return button(
        std::move(press),
        ButtonStyle{
            .pressStyle = {
                .borderWidth = 1,
                .borderPaint = Gfx::WHITE,
            }},
        handle());
}

} // namespace Karm::Ui
