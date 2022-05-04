#pragma once

#include <karm-meta/pack.h>

#include "_prelude.h"

namespace Karm::Base {

template <typename... Ts>
struct Var {
    alignas(max(alignof(Ts)...)) char _buf[max(sizeof(Ts)...)];

    uint8_t _type;

    Var() = delete;

    template <Meta::Contains<Ts...> T>
    Var(T const &value) : _type(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    Var(T &&value) : _type(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(std::move(value));
    }

    Var(Var const &other) : _type(other._type) {
        Meta::indexCast<Ts...>(
            _type, other._buf,
            [this]<typename T>(T const &ptr) {
            new (_buf) T(ptr);
            });
    }

    Var(Var &&other) : _type(other._type) {
        Meta::indexCast<Ts...>(_type, other._buf, [this]<typename T>(T &ptr) {
            new (_buf) T(std::move(ptr));
        });
    }

    ~Var() {
        Meta::indexCast<Ts...>(_type, _buf, []<typename T>(T &ptr) {
            ptr.~T();
        });
    }

    template <Meta::Contains<Ts...> T>
    Var &operator=(T const &value) {
        return *this = Var(value);
    }

    template <Meta::Contains<Ts...> T>
    Var &operator=(T &&value) {
        Meta::indexCast<Ts...>(_type, _buf, []<typename U>(U &ptr) {
            ptr.~U();
        });

        _type = Meta::indexOf<T, Ts...>();
        new (_buf) T(std::move(value));

        return *this;
    }

    Var &operator=(Var const &other) { return *this = Var(other); }

    Var &operator=(Var &&other) {
        Meta::indexCast<Ts...>(_type, _buf, []<typename T>(T &ptr) {
            ptr.~T();
        });

        _type = other._type;

        Meta::indexCast<Ts...>(_type, other._buf, [this]<typename T>(T &ptr) {
            new (_buf) T(std::move(ptr));
        });

        return *this;
    }

    template <Meta::Contains<Ts...> T>
    T &unwrap() {
        if (_type != Meta::indexOf<T, Ts...>()) {
            Debug::panic("Unwrapping wrong type");
        }

        return *static_cast<T *>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    T take() {
        if (_type != Meta::indexOf<T, Ts...>()) {
            Debug::panic("Taking wrong type");
        }

        return std::move(*static_cast<T *>(_buf));
    }

    template <Meta::Contains<Ts...> T>
    bool with(auto visitor) {
        if (_type == Meta::indexOf<T, Ts...>()) {
            visitor(*static_cast<T *>(_buf));
            return true;
        }
        return false;
    }

    void visit(auto visitor) {
        Meta::indexCast<Ts...>(_type, _buf, [&]<typename U>(U *ptr) {
            visitor(*ptr);
        });
    }

    void visit(auto visitor) const {
        Meta::indexCast<Ts...>(_type, _buf, [&]<typename U>(U const *ptr) {
            visitor(*ptr);
        });
    }

    template <Meta::Contains<Ts...> T>
    bool is() {
        return _type == Meta::indexOf<T, Ts...>();
    }
};

} // namespace Karm::Base
