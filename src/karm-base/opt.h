#pragma once

#include "_prelude.h"

#include "inert.h"
#include "keywords.h"
#include "panic.h"
#include "std.h"
#include "try.h"

namespace Karm {

template <typename T>
struct Opt {
    bool _present{};
    Inert<T> _value{};

    Opt() = default;

    Opt(None){};

    Opt(T const &value) : _present(true) {
        _value.ctor(value);
    }

    Opt(T &&value) : _present(true) {
        _value.ctor(std::move(value));
    }

    Opt(Opt const &other) : _present(other._present) {
        if (_present) {
            _value.ctor(other._value.unwrap());
        }
    }

    Opt(Opt &&other) : _present(other._present) {
        if (_present) {
            _value.ctor(other._value.take());
            other.release();
        }
    }

    ~Opt() { release(); }

    Opt &operator=(None) {
        release();
        return *this;
    }

    Opt &operator=(T const &value) {
        release();
        _present = true;
        _value.ctor(value);
        return *this;
    }

    Opt &operator=(T &&value) {
        release();
        _present = true;
        _value.ctor(std::move(value));
        return *this;
    }

    Opt &operator=(Opt const &other) {
        return *this = Opt(other);
    }

    Opt &operator=(Opt &&other) {
        release();
        if (other._present) {
            _present = true;
            _value.ctor(other._value.take());
            other.release();
        }
        return *this;
    }

    bool operator==(None) const {
        return !_present;
    }

    operator bool() const { return _present; }

    T *operator->() {
        if (!_present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    T &operator*() {
        if (!_present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    T const *operator->() const {
        if (!_present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    T const &operator*() const {
        if (!_present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    void release() {
        if (_present) {
            _value.dtor();
            _present = false;
        }
    }

    bool none() {
        return _present;
    }

    T &unwrap() {
        if (!_present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    T take() {
        if (!_present) {
            panic("Taking None");
        }
        T v = _value.take();
        release();
        return v;
    }

    bool with(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
            return true;
        }
        return false;
    }

    void visit(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
        }
    }
};

static_assert(Tryable<Opt<int>>);

} // namespace Karm
