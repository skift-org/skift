module;

#include <karm-app/event.h>
#include <karm-gfx/canvas.h>
#include <karm-math/easing.h>
#include <karm-math/flow.h>

export module Karm.Ui:anim;

import :funcs;
import :node;

namespace Karm::Ui {

export template <typename T>
struct Eased {
    T _start{};
    T _value{};
    T _target{};
    Math::Easing _easing{};
    f64 _elapsed{};
    f64 _duration{};
    f64 _delay{};
    bool _animated{};

    Eased() = default;

    T value() const {
        return _value;
    }

    T target() const {
        return _target;
    }

    void set(Node& n, T value) {
        _value = value;
        _target = value;
        _elapsed = 0;
        _duration = 0;

        _animated = true;
        Ui::shouldAnimate(n);
    }

    auto& delay(f64 delay) {
        _delay = delay;
        return *this;
    }

    void animate(Node& n, T target, f64 duration = 1.0, Math::Easing easing = {}) {
        if (Math::epsilonEq(_value, target, 0.01)) {
            _value = target;
            _target = target;
            _elapsed = 0;
            _duration = 0;
            _delay = 0;
            return;
        }

        _start = _value;
        _target = target;

        _elapsed = 0;
        _duration = duration;

        _easing = easing;
        _animated = true;

        Ui::shouldAnimate(n);
    }

    bool needRepaint(Node& n, App::Event& e) {
        if (not _animated)
            return false;

        auto ae = e.is<Node::AnimateEvent>();
        if (not ae)
            return false;

        Ui::shouldAnimate(n);

        if (_delay > 0) {
            _delay -= ae->dt;
            return false;
        }

        _elapsed += ae->dt;
        if (_elapsed > _duration) {
            _elapsed = _duration;
            _value = _target;
            _animated = false;
        } else {
            f64 p = _elapsed / _duration;
            _value = Math::lerp(_start, _target, _easing(p));
        }

        return true;
    }

    void update(Node& n, App::Event& e) {
        needRepaint(n, e);
    }

    bool reached() const {
        return not _animated;
    }
};

export using Easedi = Eased<isize>;

export using Easedf = Eased<f64>;

export template <typename T>
struct Eased2 {
    Eased<T> _x;
    Eased<T> _y;

    Eased2() = default;

    Eased2(T x, T y) : _x(x), _y(y) {}

    Eased2(Math::Vec2<T> v) : _x(v.x), _y(v.y) {}

    void set(Node& n, Math::Vec2<T> v) {
        _x.set(n, v.x);
        _y.set(n, v.y);
    }

    auto& delay(f64 delay) {
        _x.delay(delay);
        _y.delay(delay);
        return *this;
    }

    void animate(Node& n, Math::Vec2<T> target, f64 duration = 1.0, Math::Easing easing = {}) {
        _x.animate(n, target.x, duration, easing);
        _y.animate(n, target.y, duration, easing);
    }

    bool needRepaint(Node& n, App::Event& e) {
        bool sx = _x.needRepaint(n, e);
        bool sy = _y.needRepaint(n, e);
        return sx or sy;
    }

    Math::Vec2<T> value() const {
        return {_x.value(), _y.value()};
    }

    Math::Vec2<T> target() const {
        return {_x.target(), _y.target()};
    }

    bool reached() const {
        return _x.reached() and _y.reached();
    }

    T valueX() const {
        return _x.value();
    }

    T valueY() const {
        return _y.value();
    }

    T targetX() const {
        return _x.target();
    }

    T targetY() const {
        return _y.target();
    }
};

export using Eased2i = Eased2<isize>;

export using Eased2f = Eased2<f64>;

// MARK: Slide In --------------------------------------------------------------

export enum struct SlideFrom {
    START,
    END,
    TOP,
    BOTTOM,
};

struct SlideIn : ProxyNode<SlideIn> {
    SlideFrom _from;
    Easedf _slide{};

    SlideIn(SlideFrom from, Ui::Child child)
        : ProxyNode(std::move(child)),
          _from(from) {
    }

    Math::Vec2f outside() {
        switch (_from) {
        case SlideFrom::START:
            return {(f64)-bound().width, 0};

        case SlideFrom::END:
            return {(f64)bound().width, 0};

        case SlideFrom::TOP:
            return {0, (f64)-bound().height};

        case SlideFrom::BOTTOM:
            return {0, (f64)bound().height};
        }
    }

    auto translation() {
        return lerp(outside(), Math::Vec2f{}, _slide.value()).cast<isize>();
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();

        g.clip(bound());
        auto anim = translation();
        g.origin(anim.cast<f64>());
        r.xy = r.xy - anim;
        child().paint(g, r);

        g.pop();
    }

    void event(App::Event& e) override {
        if (_slide.needRepaint(*this, e)) {
            auto repaintBound =
                bound().clipTo(
                    child().bound().offset(translation())
                );

            Ui::shouldRepaint(*this, repaintBound);
        }

        Ui::ProxyNode<SlideIn>::event(e);
    }

    void attach(Node* parent) override {
        Ui::ProxyNode<SlideIn>::attach(parent);
        _slide.animate(*this, 1.0, 0.25, Math::Easing::cubicOut);
    }
};

export Child slideIn(SlideFrom from, Ui::Child child) {
    return makeRc<SlideIn>(from, std::move(child));
}

export auto slideIn(SlideFrom from) {
    return [=](Child child) {
        return slideIn(from, child);
    };
}

// MARK: Scale In --------------------------------------------------------------

export struct ScaleIn : ProxyNode<ScaleIn> {
    Easedf _scale{};

    ScaleIn(Ui::Child child)
        : ProxyNode(std::move(child)) {
    }

    Math::Vec2f scale() {
        return Math::Vec2f{0.9} + Math::Vec2f{_scale.value() * 0.1};
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(bound());
        g.origin(bound().center().cast<f64>());
        g.scale(scale());
        g.origin(-bound().center().cast<f64>());
        child().paint(g, r);
        g.pop();
    }

    void event(App::Event& e) override {
        if (_scale.needRepaint(*this, e))
            Ui::shouldRepaint(*this, bound());

        Ui::ProxyNode<ScaleIn>::event(e);
    }

    void attach(Node* parent) override {
        Ui::ProxyNode<ScaleIn>::attach(parent);
        _scale.animate(*this, 1.0, 0.25, Math::Easing::cubicOut);
    }
};

export Child scaleIn(Child child) {
    return makeRc<ScaleIn>(std::move(child));
}

export auto scaleIn() {
    return [](Child child) {
        return scaleIn(child);
    };
}

// MARK: Carousel --------------------------------------------------------------

struct Carousel : GroupNode<Carousel> {
    usize _selected;
    Math::Flow _flow;
    Easedf _slide{};

    Carousel(usize selected, Children children, Math::Flow flow)
        : GroupNode(children), _selected(selected), _flow(flow) {
    }

    void reconcile(Carousel& o) override {
        GroupNode::reconcile(o);
        if (_selected != o._selected) {
            _selected = o._selected;
            _slide.animate(*this, _selected, 0.3, Math::Easing::cubicOut);
        }
    }

    Math::Vec2i translation() {
        return {
            (int)(-_slide.value() * bound().width),
            0,
        };
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(bound());
        auto anim = translation();
        g.origin(anim.cast<f64>());
        for (auto& child : children()) {
            child->paint(g, r);
        }
        g.pop();
    }

    void event(App::Event& e) override {
        if (_slide.needRepaint(*this, e)) {
            Ui::shouldRepaint(*this, bound());
        }

        GroupNode::event(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        for (auto& child : children()) {
            child->layout(r);
            r = r.offset({r.width, 0});
        }
    }
};

export Child carousel(usize selected, Children children, Math::Flow flow) {
    return makeRc<Carousel>(selected, std::move(children), flow);
}

} // namespace Karm::Ui
