#pragma once

#include <karm-meta/pack.h>

#include "_prelude.h"

#include "clamp.h"
#include "opt.h"
#include "panic.h"

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
    ALWAYS_INLINE T &unwrap() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            panic("Unwrapping wrong type");
        }

        return *reinterpret_cast<T *>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE T const &unwrap() const {
        if (_index != Meta::indexOf<T, Ts...>()) {
            panic("Unwrapping wrong type");
        }

        return *reinterpret_cast<T const *>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE Opt<T> take() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            return NONE;
        }

        return std::move(*reinterpret_cast<T *>(_buf));
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE Opt<T> take() const {
        if (_index != Meta::indexOf<T, Ts...>()) {
            return NONE;
        }

        return *reinterpret_cast<T const *>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE bool with(auto visitor) {
        if (_index == Meta::indexOf<T, Ts...>()) {
            visitor(*reinterpret_cast<T *>(_buf));
            return true;
        }
        return false;
    }

    ALWAYS_INLINE auto visit(auto visitor) {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    ALWAYS_INLINE auto visit(auto visitor) const {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE bool is() const {
        return _index == Meta::indexOf<T, Ts...>();
    }

    ALWAYS_INLINE size_t index() const { return _index; }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE bool operator==(T const &other) const {
        return is<T>() && unwrap<T>() == other;
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE bool operator!=(T const &other) const {
        return !(*this == other);
    }

    template <Meta::Contains<Ts...> T>
    ALWAYS_INLINE Ordr cmp(T const &other) const {
        if (is<T>()) {
            return ::cmp(unwrap<T>(), other);
        }

        return Ordr::LESS;
    }

    ALWAYS_INLINE Ordr cmp(Var const &other) const {
        if (_index == other._index) {
            return visit([&]<typename T>(T const &ptr) {
                return ::cmp(ptr, other.unwrap<T>());
            });
        }

        return Ordr::LESS;
    }
};

template <typename... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

} // namespace Karm
