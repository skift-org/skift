#pragma once

#include "_prelude.h"

#include "inert.h"
#include "std.h"

namespace Karm::Base {

struct None {
};

static constexpr None NONE = None{};

template <typename T>
struct Opt {
    bool _present = false;

    Inert<T> _value;

    Opt(){};

    Opt(None){};

    Opt(T const &value) : _present(true) {
        _value.construct(value);
    }

    Opt(T &&value) : _present(true) {
        _value.construct(std::forward<T>(value));
    }

    Opt(Opt const &other) : _present(other._present) {
        if (_present) {
            _value.construct(other._value);
        }
    }

    Opt(Opt &&other) : _present(other._present) {
        if (_present) {
            _value.construct(std::forward<T>(other._value.unwrap()));
            other.release();
        }
    }

    ~Opt() { release(); }

    Opt &operator=(Opt const &other) { return *this = Opt(other); }

    Opt &operator=(Opt &&other) {
        release();
        if (other._present) {
            _value.construct(std::forward<T>(other._value.unwrap()));
            other.release();
        }
        return *this;
    }

    operator bool() const { return _present; }

    T *operator->() { return &_value; }

    T &operator*() { return *_value; }

    T const *operator->() const { return &_value; }

    T const &operator*() const { return *_value; }

    void release() {
        if (_present) {
            _value.destroy();
            _present = false;
        }
    }

    bool error() {
        return _present;
    }

    T unwrap() {
        T result = std::move(_value.unwrap());
        return result;
    }

    void with(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
        }
    }

    void visit(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
        }
    }
};

} // namespace Karm::Base
