#pragma once

#include <karm-math/easing.h>

#include "funcs.h"
#include "node.h"

namespace Karm::Ui {

template <typename T>
struct Anim {
    T _start{};
    T _value{};
    T _target{};
    Math::Easing _easing{};
    double _elapsed{0};
    double _duration{0};
    bool _animated = false;

    Anim() = default;

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

    void animate(Node &n, T target, double duration = 1.0, Math::Easing easing = {}) {
        _start = _value;
        _target = target;

        _elapsed = 0;
        _duration = duration;

        _easing = easing;
        _animated = true;

        Ui::shouldAnimate(n);
    }

    void update(double dt) {
        if (_animated) {
            _elapsed += dt;
            if (_elapsed > _duration) {
                _elapsed = _duration;
                _value = _target;
                _animated = false;
            } else {
                double p = _elapsed / _duration;
                _value = Math::lerp(_start, _target, _easing(p));
            }
        }
    }

    void event(Node &n, Events::Event &e) {
        e.handle<Events::AnimateEvent>([&](auto &) {
            if (_animated) {
                Ui::shouldAnimate(n);
                Ui::shouldRepaint(n);
            }
            update(1 / 60.0);
            return false;
        });
    }

    bool reached() const {
        return !_animated;
    }

    operator T() const {
        return _value;
    }

    auto const &operator*() const {
        return _value;
    }

    auto *operator->() const {
        return &_value;
    }
};

template <typename T>
struct Anim2 {
    Anim<T> _x;
    Anim<T> _y;

    Anim2() = default;

    Anim2(T x, T y) : _x{x}, _y{y} {}

    Anim2(Math::Vec2<T> v) : _x{v.x}, _y{v.y} {}

    void set(Node &n, Math::Vec2<T> v) {
        _x.set(n, v.x);
        _y.set(n, v.y);
    }

    void animate(Node &n, Math::Vec2<T> target, double duration = 1.0, Math::Easing easing = {}) {
        _x.animate(n, target.x, duration, easing);
        _y.animate(n, target.y, duration, easing);
    }

    void update(double dt) {
        _x.update(dt);
        _y.update(dt);
    }

    void event(Node &n, Events::Event &e) {
        _x.event(n, e);
        _y.event(n, e);
    }

    Math::Vec2<T> value() const {
        return {_x.value(), _y.value()};
    }

    Math::Vec2<T> target() const {
        return {_x.target(), _y.target()};
    }

    bool reached() const {
        return _x.reached() && _y.reached();
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

using Anim2i = Anim2<int>;

using Anim2f = Anim2<double>;

} // namespace Karm::Ui
