#pragma once

#include <karm-meta/pack.h>

#include "_prelude.h"

namespace Karm {

template <typename... Ts>
struct Var {
    alignas(max(alignof(Ts)...)) char _buf[max(sizeof(Ts)...)];
    uint8_t _index;

    Var() = delete;

    template <Meta::Contains<Ts...> T>
    Var(T const &value) : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    Var(T &&value) : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(std::move(value));
    }

    Var(Var const &other) : _index(other._index) {
        Meta::indexCast<Ts...>(
            _index, other._buf,
            [this]<typename T>(T const &ptr) {
                new (_buf) T(ptr);
            });
    }

    Var(Var &&other) : _index(other._index) {
        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T &ptr) {
            new (_buf) T(std::move(ptr));
        });
    }

    ~Var() {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T &ptr) {
            ptr.~T();
        });
    }

    template <Meta::Contains<Ts...> T>
    Var &operator=(T const &value) {
        return *this = Var(value);
    }

    template <Meta::Contains<Ts...> T>
    Var &operator=(T &&value) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U &ptr) {
            ptr.~U();
        });

        _index = Meta::indexOf<T, Ts...>();
        new (_buf) T(std::move(value));

        return *this;
    }

    Var &operator=(Var const &other) { return *this = Var(other); }

    Var &operator=(Var &&other) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T &ptr) {
            ptr.~T();
        });

        _index = other._index;

        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T &ptr) {
            new (_buf) T(std::move(ptr));
        });

        return *this;
    }

    template <Meta::Contains<Ts...> T>
    T &unwrap() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            Debug::panic("Unwrapping wrong type");
        }

        return *static_cast<T *>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    T take() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            Debug::panic("Taking wrong type");
        }

        return std::move(*static_cast<T *>(_buf));
    }

    template <Meta::Contains<Ts...> T>
    bool with(auto visitor) {
        if (_index == Meta::indexOf<T, Ts...>()) {
            visitor(*static_cast<T *>(_buf));
            return true;
        }
        return false;
    }

    void visit(auto visitor) {
        Meta::indexCast<Ts...>(_index, _buf, [&]<typename U>(U *ptr) {
            visitor(*ptr);
        });
    }

    void visit(auto visitor) const {
        Meta::indexCast<Ts...>(_index, _buf, [&]<typename U>(U const *ptr) {
            visitor(*ptr);
        });
    }

    template <Meta::Contains<Ts...> T>
    bool is() {
        return _index == Meta::indexOf<T, Ts...>();
    }

    size_t index() const { return _index; }
};

template <typename... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

} // namespace Karm
