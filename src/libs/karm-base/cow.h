#pragma once

#include <karm-base/rc.h>

namespace Karm {

template <typename T>
struct Cow {
    Rc<T> _inner = base();

    static Opt<Rc<T>> _base;

    static Rc<T> base() {
        if (not _base) {
            _base = makeRc<T>();
        }
        return _base.unwrap();
    }

    T& cow() {
        if (_inner.refs() > 1)
            _inner = makeRc<T>(_inner.unwrap());
        return _inner.unwrap();
    }

    constexpr T const* operator->() const {
        return &_inner.unwrap();
    }

    constexpr T const& operator*() const {
        return _inner.unwrap();
    }

    bool sameInstance(Cow const& other) const {
        return &_inner == &other._inner;
    }
};

template <typename T>
Opt<Rc<T>> Cow<T>::_base = NONE;

template <typename T>
Cow<T> makeCow(T const& value) {
    return {makeRc<T>(value)};
}

} // namespace Karm
