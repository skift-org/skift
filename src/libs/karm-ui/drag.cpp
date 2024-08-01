#include <karm-ui/drag.h>

namespace Karm::Ui {

// MARK: Dismisable ------------------------------------------------------------

struct Dismisable :
    public ProxyNode<Dismisable> {

    OnDismis _onDismis;
    DismisDir _dir;
    f64 _threshold;

    Eased2f _drag{};
    Math::Vec2i _last{};
    bool _dismissed{};

    Dismisable(OnDismis onDismis, DismisDir dir, f64 threshold, Ui::Child child)
        : ProxyNode(child),
          _onDismis(std::move(onDismis)),
          _dir(dir),
          _threshold(threshold) {}

    void reconcile(Dismisable &o) override {
        _onDismis = std::move(o._onDismis);
        _dir = o._dir;
        _threshold = o._threshold;

        ProxyNode<Dismisable>::reconcile(o);
    }

    Math::Vec2i drag() const {
        return _drag.value().cast<isize>();
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        g.clip(bound());
        g.origin(drag());
        r.xy = r.xy - drag();
        child().paint(g, r);

        g.restore();
    }

    void event(Sys::Event &e) override {
        if (auto *me = e.is<Events::MouseEvent>()) {
            me->pos = me->pos - drag();
            child().event(e);
            me->pos = me->pos + drag();
        } else if (e.is<Node::AnimateEvent>() and _dismissed and _drag.reached()) {
            _onDismis(*this);
            _dismissed = false;
            Ui::ProxyNode<Dismisable>::event(e);
        } else if (_drag.needRepaint(*this, e)) {
            auto childBound = child().bound();
            auto repaintBound =
                bound()
                    .clipTo(childBound
                                .offset(_last)
                                .mergeWith(childBound.offset(drag())));
            _last = drag();
            Ui::shouldRepaint(*this, repaintBound);
            Ui::ProxyNode<Dismisable>::event(e);
        } else {
            Ui::ProxyNode<Dismisable>::event(e);
        }
    }

    void bubble(Sys::Event &e) override {
        if (auto *de = e.is<DragEvent>()) {
            if (de->type == DragEvent::DRAG) {
                auto d = _drag.target() + de->delta;

                d.x = clamp(
                    d.x,
                    (bool)(_dir & DismisDir::LEFT) ? -bound().width : 0,
                    (bool)(_dir & DismisDir::RIGHT) ? bound().width : 0
                );

                d.y = clamp(
                    d.y,
                    (bool)(_dir & DismisDir::TOP) ? -bound().height : 0,
                    (bool)(_dir & DismisDir::DOWN) ? bound().height : 0
                );

                _drag.set(*this, d);
            } else if (de->type == DragEvent::END) {
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

// MARK: Drag Region -----------------------------------------------------------

struct DragRegion : public ProxyNode<DragRegion> {
    bool _grabbed{};

    using ProxyNode::ProxyNode;

    void event(Sys::Event &event) override {
        ProxyNode::event(event);

        if (event.accepted())
            return;

        auto *e = event.is<Events::MouseEvent>();
        if (not e)
            return;

        if (not bound().contains(e->pos) and not _grabbed)
            return;

        if (e->type == Events::MouseEvent::PRESS) {
            _grabbed = true;
            bubble<DragEvent>(*this, DragEvent::START);
            event.accept();
        } else if (e->type == Events::MouseEvent::RELEASE) {
            _grabbed = false;
            bubble<DragEvent>(*this, DragEvent::END);
            event.accept();
        } else if (e->type == Events::MouseEvent::MOVE and _grabbed) {
            bubble<DragEvent>(*this, DragEvent::DRAG, e->delta);
            event.accept();
        }
    }
};

Child dragRegion(Child child) {
    return makeStrong<DragRegion>(child);
}

// MARK: Handle ----------------------------------------------------------------

Child handle() {
    return empty({128, 4}) |
           box({
               .borderRadii = 999,
               .backgroundPaint = GRAY50,
           }) |
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
