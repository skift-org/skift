#pragma once

#include <karm-debug/logger.h>
#include <karm-debug/panic.h>

#include "_prelude.h"

#include "inert.h"
#include "keywords.h"
#include "macros.h"
#include "std.h"

namespace Karm {

template <typename T>
struct Opt {
    bool _present = false;

    Inert<T> _value;

    Opt(){};

    Opt(None){};

    Opt(T const &value) : _present(true) {
        _value.ctor(value);
    }

    Opt(T &&value) : _present(true) {
        _value.ctor(std::forward<T>(value));
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
        _value.ctor(std::forward<T>(value));
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

    T *operator->() { return &_value; }

    T &operator*() { return *_value; }

    T const *operator->() const { return &_value; }

    T const &operator*() const { return *_value; }

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
            Debug::panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    T take() {
        if (!_present) {
            Debug::panic("Taking None");
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
