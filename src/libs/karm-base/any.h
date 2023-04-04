#pragma once

#include <karm-meta/id.h>

#include "panic.h"

namespace Karm {

struct _Ops {
    void (*dtor)(void *);
    void (*copy)(void *, void *);

    template <typename T>
    static _Ops make() {
        _Ops ops;
        ops.dtor = [](void *ptr) {
            delete static_cast<T *>(ptr);
        };

        ops.copy = [](void *dst, void *src) {
            new (dst) T(*static_cast<T *>(src));
        };

        return ops;
    }
};

struct Any {
    Meta::Id _type{};
    void *_buf{};
    _Ops _ops{};

    Any() = delete;

    template <typename T>
    Any(T const &value)
        : _type(Meta::idOf<T>()),
          _buf(new T(value)),
          _ops(_Ops::make<T>()) {}

    template <typename T>
    Any(T &&value)
        : _type(Meta::idOf<T>()),
          _buf(new T(std::move(value))),
          _ops(_Ops::make<T>()) {}

    Any(Any const &other)
        : _type(other._type),
          _buf(other._buf),
          _ops(other._ops) {
        _ops.copy(_buf, other._buf);
    }

    Any(Any &&other)
        : _type(other._type),
          _buf(other._buf),
          _ops(other._ops) {
        other._buf = nullptr;
    }

    ~Any() {
        _ops.dtor(_buf);
    }

    template <typename T>
    Any &operator=(T const &value) {
        *this = Any(value);
        return *this;
    }

    template <typename T>
    Any &operator=(T &&value) {
        _ops.dtor(_buf);

        _type = Meta::idOf<T>();
        _buf = new T(std::move(value));
        _ops = _Ops::make<T>();

        return *this;
    }

    Any &operator=(Any const &other) {
        *this = Any(other);
        return *this;
    }

    Any &operator=(Any &&other) {
        _ops.dtor(_buf);

        _type = other._type;
        _buf = other._buf;
        _ops = other._ops;

        other._buf = nullptr;

        return *this;
    }

    template <typename T>
    T &unwrap() {
        if (_type != Meta::idOf<T>()) {
            panic("Unwrapping wrong type");
        }
        return *static_cast<T *>(_buf);
    }

    template <typename T>
    T const &unwrap() const {
        if (_type != Meta::idOf<T>()) {
            panic("Unwrapping wrong type");
        }
        return *static_cast<T *>(_buf);
    }

    template <typename T>
    bool is() const {
        return _type == Meta::idOf<T>();
    }
};

} // namespace Karm
