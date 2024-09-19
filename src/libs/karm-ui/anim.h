#pragma once

#include <karm-math/easing.h>
#include <karm-math/flow.h>

#include "funcs.h"
#include "node.h"

namespace Karm::Ui {

template <typename T>
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

    void set(Node &n, T value) {
        _value = value;
        _target = value;
        _elapsed = 0;
        _duration = 0;

        _animated = true;
        Ui::shouldAnimate(n);
    }

    auto &delay(f64 delay) {
        _delay = delay;
        return *this;
    }

    void animate(Node &n, T target, f64 duration = 1.0, Math::Easing easing = {}) {
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

    bool needRepaint(Node &n, App::Event &event) {
        if (not _animated)
            return false;

        auto e = event.is<Node::AnimateEvent>();
        if (not e)
            return false;

        Ui::shouldAnimate(n);

        if (_delay > 0) {
            _delay -= e->dt;
            return false;
        }

        _elapsed += e->dt;
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

    void update(Node &n, App::Event &e) {
        needRepaint(n, e);
    }

    bool reached() const {
        return not _animated;
    }
};

using Easedi = Eased<isize>;

using Easedf = Eased<f64>;

template <typename T>
struct Eased2 {
    Eased<T> _x;
    Eased<T> _y;

    Eased2() = default;

    Eased2(T x, T y) : _x(x), _y(y) {}

    Eased2(Math::Vec2<T> v) : _x(v.x), _y(v.y) {}

    void set(Node &n, Math::Vec2<T> v) {
        _x.set(n, v.x);
        _y.set(n, v.y);
    }

    auto &delay(f64 delay) {
        _x.delay(delay);
        _y.delay(delay);
        return *this;
    }

    void animate(Node &n, Math::Vec2<T> target, f64 duration = 1.0, Math::Easing easing = {}) {
        _x.animate(n, target.x, duration, easing);
        _y.animate(n, target.y, duration, easing);
    }

    bool needRepaint(Node &n, App::Event &e) {
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

using Eased2i = Eased2<isize>;

using Eased2f = Eased2<f64>;

// MARK: Slide In --------------------------------------------------------------

enum struct SlideFrom {
    START,
    END,
    TOP,
    BOTTOM,
};

Child slideIn(SlideFrom from, Child child);

inline auto slideIn(SlideFrom from) {
    return [=](Child child) {
        return slideIn(from, child);
    };
}

// MARK: Scale In --------------------------------------------------------------

Child scaleIn(Child child);

inline auto scaleIn() {
    return [](Child child) {
        return scaleIn(child);
    };
}

// MARK: Carousel --------------------------------------------------------------

Child carousel(usize selected, Children children, Math::Flow flow = Math::Flow::LEFT_TO_RIGHT);

} // namespace Karm::Ui
