#include <karm-ui/drag.h>

namespace Karm::Ui {

/* --- Dismisable ----------------------------------------------------------- */

struct Dismisable :
    public ProxyNode<Dismisable> {

    OnDismis _onDismis;
    DismisDir _dir;
    f64 _threshold;
    Anim2<f64> _drag{};
    bool _dismissed{};
    bool _animated{};

    Dismisable(OnDismis onDismis, DismisDir dir, f64 threshold, Ui::Child child)
        : ProxyNode(child), _onDismis(std::move(onDismis)), _dir(dir), _threshold(threshold) {}

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        auto dragi = _drag.value().cast<isize>();

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
                    if (Math::abs(_drag.targetX()) / (f64)bound().width > _threshold) {
                        _drag.animate(*this, {bound().width * (_drag.targetX() < 0.0 ? -1.0 : 1), 0}, 0.25, Math::Easing::cubicOut);
                        _dismissed = true;
                    } else {
                        _drag.animate(*this, {0, _drag.targetY()}, 0.25, Math::Easing::exponentialOut);
                    }
                }
                if ((bool)(_dir & DismisDir::VERTICAL)) {
                    if (Math::abs(_drag.targetY()) / (f64)bound().height > _threshold) {
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

Child dismisable(OnDismis onDismis, DismisDir dir, f64 threshold, Ui::Child child) {
    return makeStrong<Dismisable>(std::move(onDismis), dir, threshold, std::move(child));
}

/* --- Drag Region ---------------------------------------------------------- */

struct DragRegion : public ProxyNode<DragRegion> {
    bool _grabbed{};

    using ProxyNode::ProxyNode;

    void event(Events::Event &e) override {
        if (not _grabbed)
            _child->event(e);

        if (e.accepted)
            return;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (not bound().contains(m.pos) and not _grabbed) {
                return false;
            }

            if (m.type == Events::MouseEvent::PRESS) {
                _grabbed = true;

                DragEvent de = {.type = DragEvent::START};
                bubble(de);

                return true;
            }

            if (m.type == Events::MouseEvent::RELEASE) {
                _grabbed = false;

                DragEvent de = {.type = DragEvent::END};
                bubble(de);

                return true;
            }

            if (m.type == Events::MouseEvent::MOVE) {
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

Child dragRegion(Child child) {
    return makeStrong<DragRegion>(child);
}

/* --- Handle --------------------------------------------------------------- */

Child handle() {
    BoxStyle STYLE = {
        .borderRadius = 999,
        .backgroundPaint = Gfx::WHITE,
    };

    return empty({128, 4}) |
           box(STYLE) |
           spacing(12) |
           center() |
           minSize({UNCONSTRAINED, 48});
}

Child dragHandle() {
    return handle() | dragRegion();
}

Child buttonHandle(OnPress press) {
    return handle() |
           button(std::move(press), Ui::ButtonStyle::none());
}

} // namespace Karm::Ui
