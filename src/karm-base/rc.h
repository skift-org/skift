#pragma once

#include <karm-debug/panic.h>
#include <karm-meta/traits.h>

#include "_prelude.h"

#include "opt.h"
#include "ref.h"
#include "std.h"

namespace Karm::Base {

struct _Rc {
    int _strong = 0;
    int _weak = 0;

    virtual ~_Rc() = default;
    virtual void *_unwrap() = 0;

    bool dying() {
        return _strong == 0;
    }

    _Rc *collect() {
        if (_strong == 0 && _weak == 0) {
            delete this;
            return nullptr;
        }

        return this;
    }

    _Rc *refStrong() {
        _strong++;
        return this;
    }

    _Rc *derefStrong() {
        _strong--;
        return collect();
    }

    template <typename T>
    T &unwrapStrong() {
        return *static_cast<T *>(_unwrap());
    }

    _Rc *refWeak() {
        _weak++;
        return this;
    }

    _Rc *derefWeak() {
        _weak--;
        return collect();
    }

    template <typename T>
    _Rc *unwrapWeak() {
        if (_strong == 0) {
            Debug::panic("Dereferencing weak reference to dead object");
        }

        return *static_cast<T *>(_unwrap());
    }
};

template <typename T>
struct Rc : public _Rc {
    T _buf;
    template <typename... Args>
    Rc(Args &&...args) : _buf(std::forward<Args>(args)...) {}

    void *_unwrap() override { return &_buf; }
};

template <typename T>
struct Strong {
    _Rc *_rc = nullptr;

    constexpr Strong() = delete;

    constexpr Strong(_Rc *ptr) : _rc(ptr->refStrong()) {}

    constexpr Strong(Strong const &other) : _rc(other._rc->refStrong()) {}

    constexpr Strong(Strong &&other) : _rc(std::exchange(other._rc, nullptr)) {}

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> const &other) : _rc(other._rc->refStrong()) {}

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> &&other) : _rc(std::exchange(other._rc, nullptr)) {}

    constexpr ~Strong() {
        if (_rc) {
            _rc = _rc->derefStrong();
        }
    }

    constexpr Strong &operator=(Strong const &other) {
        return *this = Strong(other);
    }

    constexpr Strong &operator=(Strong &&other) {
        std::swap(_rc, other._rc);
        return *this;
    }

    constexpr T *operator->() const {
        if (!_rc) {
            Debug::panic("Deferencing moved from Strong<T>");
        }

        return &_rc->unwrapStrong<T>();
    }

    constexpr T &operator*() const {
        if (!_rc) {
            Debug::panic("Deferencing moved from Strong<T>");
        }

        return _rc->unwrapStrong<T>();
    }
};

template <typename T>
using OptStrong = Opt<Strong<T>>;

template <typename T, typename... Args>
constexpr static Strong<T> makeStrong(Args... args) {
    return {new Rc<T>(std::forward<Args>(args)...)};
}

template <typename T>
struct Weak {
    _Rc *_rc = nullptr;

    constexpr Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr Weak(Strong<U> const &other) : _rc(other._rc->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> const &other) : _rc(other._rc->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> &&other) { std::swap(_rc, other._rc); }

    constexpr ~Weak() {
        if (_rc) {
            _rc->derefWeak();
        }
    }

    constexpr Weak &operator=(Weak const &other) {
        return *this = Weak(other);
    }

    constexpr Weak &operator=(Weak &&other) {
        std::swap(_rc, other._rc);
        return *this;
    }

    operator bool() {
        return _rc && _rc->dying() && (_rc = _rc->derefWeak());
    }

    operator bool() const {
        return _rc && _rc->dying();
    }

    T *operator->() {
        if (!_rc)
            Debug::panic("Deferencing moved from Weak<T>");

        return _rc->unwrapWeak<T>();
    }

    T &unwrap() {
        if (!_rc)
            Debug::panic("Deferencing moved from Weak<T>");

        return _rc->unwrapWeak<T>();
    }

    void visit(auto visitor) {
        if (*this) {
            visitor(**this);
        }
    }
};

template <typename T>
using OptWeak = Opt<Weak<T>>;

} // namespace Karm::Base
