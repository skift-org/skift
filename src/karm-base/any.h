#pragma once

#include <karm-debug/panic.h>
#include <karm-meta/id.h>

namespace Karm::Base {

struct _Ops {
    void (*dtor)(void *);
    void (*copy)(void *, void *);

    template <typename T>
    static auto make() -> _Ops {
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
    Meta::Id _type;
    void *_buf;
    _Ops _ops;

    Any() = delete;

    template <typename T>
    Any(T const &value) : _type(Meta::make_id<T>()), _buf(new T(value)), _ops(_Ops::make<T>()) {}

    template <typename T>
    Any(T &&value) : _type(Meta::make_id<T>()), _buf(new T(std::move(value))), _ops(_Ops::make<T>()) {}

    Any(Any const &other) : _type(other._type), _buf(other._buf), _ops(other._ops) {
        _ops.copy(_buf, other._buf);
    }

    Any(Any &&other) : _type(other._type), _buf(other._buf), _ops(other._ops) {
        other._buf = nullptr;
    }

    ~Any() {
        _ops.dtor(_buf);
    }

    template <typename T>
    auto operator=(T const &value) -> Any & {
        return *this = Any(value);
    }

    template <typename T>
    auto operator=(T &&value) -> Any & {
        _ops.dtor(_buf);

        _type = Meta::make_id<T>();
        _buf = new T(std::move(value));
        _ops = _Ops::make<T>();

        return *this;
    }

    auto operator=(Any const &other) -> Any & {
        return *this = Any(other);
    }

    auto operator=(Any &&other) -> Any & {
        _ops.dtor(_buf);

        _type = other._type;
        _buf = other._buf;
        _ops = other._ops;

        other._buf = nullptr;

        return *this;
    }

    template <typename T>
    auto unwrap() -> T & {
        if (_type != Meta::make_id<T>()) {
            Debug::panic("Unwrapping wrong type");
        }
        return *static_cast<T *>(_buf);
    }

    template <typename T>
    auto unwrap() const -> T const & {
        if (_type != Meta::make_id<T>()) {
            Debug::panic("Unwrapping wrong type");
        }
        return *static_cast<T *>(_buf);
    }

    template <typename T>
    operator T &() {
        return unwrap<T>();
    }

    template <typename T>
    operator T const &() const {
        return unwrap<T>();
    }

    template <typename T>
    auto is() const -> bool {
        return _type == Meta::make_id<T>();
    }
};

} // namespace Karm::Base
