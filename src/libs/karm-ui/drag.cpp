module;

#include <karm-gfx/canvas.h>
#include <karm-math/easing.h>

export module Karm.Ui:drag;

import Karm.App;
import :anim;
import :layout;
import :view;
import :box;

namespace Karm::Ui {

// MARK: Drag Event ------------------------------------------------------------

export struct DragEvent {
    enum {
        START,
        DRAG,
        END
    } type;

    Math::Vec2i delta{};
};

// MARK: Dismisable ------------------------------------------------------------

export enum struct DismisDir {
    LEFT = 1 << 0,
    RIGHT = 1 << 1,
    TOP = 1 << 2,
    DOWN = 1 << 3,

    HORIZONTAL = LEFT | RIGHT,
    VERTICAL = TOP | DOWN,
};

struct Dismisable :
    ProxyNode<Dismisable> {

    Send<> _onDismis;
    Flags<DismisDir> _dir;
    f64 _threshold;

    Eased2f _drag{};
    Math::Vec2i _last{};
    bool _dismissed{};

    Dismisable(Send<> onDismis, Flags<DismisDir> dir, f64 threshold, Ui::Child child)
        : ProxyNode(child),
          _onDismis(std::move(onDismis)),
          _dir(dir),
          _threshold(threshold) {}

    void reconcile(Dismisable& o) override {
        _onDismis = std::move(o._onDismis);
        _dir = o._dir;
        _threshold = o._threshold;

        ProxyNode<Dismisable>::reconcile(o);
    }

    Math::Vec2i drag() const {
        return _drag.value().cast<isize>();
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();

        g.clip(bound());
        g.origin(drag().cast<f64>());
        r.xy = r.xy - drag();
        child().paint(g, r);

        g.pop();
    }

    void event(App::Event& e) override {
        if (auto me = e.is<App::MouseEvent>()) {
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

    void bubble(App::Event& e) override {
        if (auto de = e.is<DragEvent>()) {
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
                e.accept();
            }

            if (de->type == DragEvent::END) {
                if ((bool)(_dir & DismisDir::HORIZONTAL)) {
                    if (Math::abs(_drag.targetX()) / (f64)bound().width > _threshold) {
                        _drag.animate(*this, {bound().width * (_drag.targetX() < 0.0 ? -1.0 : 1), 0}, 0.25, Math::Easing::cubicOut);
                        _dismissed = true;
                    } else {
                        _drag.animate(*this, {0, _drag.targetY()}, 0.25, Math::Easing::exponentialOut);
                    }
                    e.accept();
                }
                if ((bool)(_dir & DismisDir::VERTICAL)) {
                    if (Math::abs(_drag.targetY()) / (f64)bound().height > _threshold) {
                        _drag.animate(*this, {0, bound().height * (_drag.targetY() < 0.0 ? -1.0 : 1)}, 0.25, Math::Easing::cubicOut);
                        _dismissed = true;
                    } else {
                        _drag.animate(*this, {_drag.targetX(), 0}, 0.25, Math::Easing::exponentialOut);
                    }
                    e.accept();
                }
            }
        }

        Ui::ProxyNode<Dismisable>::bubble(e);
    }
};

export Child dismisable(Send<> onDismis, Flags<DismisDir> dir, f64 threshold, Ui::Child child) {
    return makeRc<Dismisable>(onDismis, dir, threshold, std::move(child));
}

export auto dismisable(Send<> onDismis, Flags<DismisDir> dir, f64 threshold) {
    return [=](Ui::Child child) mutable {
        return dismisable(onDismis, dir, threshold, child);
    };
}

// MARK: Drag Region -----------------------------------------------------------

struct DragRegion : ProxyNode<DragRegion> {
    bool _grabbed{};
    Math::Vec2i _dir;

    DragRegion(Child child, Math::Vec2i dir)
        : ProxyNode(child),
          _dir(dir) {}

    using ProxyNode::ProxyNode;

    void event(App::Event& event) override {
        ProxyNode::event(event);

        if (event.accepted())
            return;

        auto e = event.is<App::MouseEvent>();
        if (not e)
            return;

        if (not bound().contains(e->pos) and not _grabbed)
            return;

        if (e->type == App::MouseEvent::PRESS) {
            _grabbed = true;
            bubble<DragEvent>(*this, DragEvent::START);
            event.accept();
        } else if (e->type == App::MouseEvent::RELEASE) {
            _grabbed = false;
            bubble<DragEvent>(*this, DragEvent::END);
            event.accept();
        } else if (e->type == App::MouseEvent::MOVE and _grabbed) {
            bubble<DragEvent>(*this, DragEvent::DRAG, e->delta * _dir);
            event.accept();
        }
    }
};

export Child dragRegion(Child child, Math::Vec2i dir) {
    return makeRc<DragRegion>(child, dir);
}

export auto dragRegion(Math::Vec2i dir = {1, 1}) {
    return [dir](Child child) {
        return dragRegion(child, dir);
    };
}

} // namespace Karm::Ui
