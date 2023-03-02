#pragma once

#include <karm-meta/callable.h>

#include "error.h"
#include "inert.h"
#include "panic.h"
#include "std.h"
#include "try.h"

namespace Karm {

template <typename T>
struct Opt;

template <typename T>
struct [[nodiscard]] Opt {
    bool _present = false;
    Inert<T> _value{};

    Opt() {}

    Opt(None) {}

    template <typename U = T>
    Opt(U &&value)
        requires(!Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
        : _present(true) {
        _value.ctor(std::forward<U>(value));
    }

    Opt(Opt const &other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present) {
            _value.ctor(other.unwrap());
        }
    }

    Opt(Opt &&other) : _present(other._present) {
        if (_present) {
            _value.ctor(other.take());
        }
    }

    ~Opt() {
        release();
    }

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
        *this = Opt(other);
        return *this;
    }

    Opt &operator=(Opt &&other) {
        release();
        if (other._present) {
            _present = true;
            _value.ctor(other.take());
        }
        return *this;
    }

    bool operator==(None) const {
        return not _present;
    }

    operator bool() const {
        return _present;
    }

    T *operator->() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    T &operator*() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    T const *operator->() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    T const &operator*() const {
        if (not _present) {
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

    None none() const {
        return NONE;
    }

    T &unwrap(const char *msg = "unwraping none") {
        if (not _present) {
            panic(msg);
        }
        return _value.unwrap();
    }

    T const &unwrap(const char *msg = "unwraping none") const {
        if (not _present) {
            panic(msg);
        }
        return _value.unwrap();
    }

    T take() {
        if (not _present) {
            panic("taking from none");
        }
        T v = _value.take();
        _present = false;
        return v;
    }

    bool with(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
            return true;
        }
        return false;
    }

    bool with(auto visitor) const {
        if (_present) {
            visitor(_value.unwrap());
            return true;
        }
        return false;
    }

    auto visit(auto visitor) -> decltype(visitor(_value.unwrap())) {
        if (_present) {
            return visitor(_value.unwrap());
        }

        return {};
    }

    auto visit(auto visitor) const -> decltype(visitor(_value.unwrap())) {
        if (_present) {
            return visitor(_value.unwrap());
        }

        return {};
    }

    // call operator
    template <typename... Args>
    auto operator()(Args &&...args) {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            if (not _present) {
                return false;
            }
            _value.unwrap()(std::forward<Args>(args)...);
            return true;
        } else {
            if (not _present) {
                return OptRet{NONE};
            }

            return OptRet{_value.unwrap()(std::forward<Args>(args)...)};
        }
    }
};

static_assert(Tryable<Opt<isize>>);

} // namespace Karm
