#pragma once

#include <karm-meta/pack.h>

#include "clamp.h"
#include "opt.h"
#include "std.h"

namespace Karm {

template <typename... Ts>
struct Var {
    static_assert(sizeof...(Ts) <= 255, "Var can only hold up to 255 types");

    alignas(max(alignof(Ts)...)) char _buf[max(sizeof(Ts)...)];
    u8 _index;

    always_inline Var() = delete;

    template <Meta::Contains<Ts...> T>
    always_inline Var(T const &value)
        : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    always_inline Var(T &&value)
        : _index(Meta::indexOf<T, Ts...>()) {
        new (_buf) T(std::move(value));
    }

    always_inline Var(Var const &other)
        : _index(other._index) {
        Meta::indexCast<Ts...>(
            _index, other._buf,
            [this]<typename T>(T const &ptr) {
                new (_buf) T(ptr);
            });
    }

    always_inline Var(Var &&other)
        : _index(other._index) {
        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T &ptr) {
            new (_buf) T(std::move(ptr));
        });
    }

    always_inline ~Var() {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T &ptr) {
            ptr.~T();
        });
    }

    template <Meta::Contains<Ts...> T>
    always_inline Var &operator=(T const &value) {
        *this = Var(value);
        return *this;
    }

    template <Meta::Contains<Ts...> T>
    always_inline Var &operator=(T &&value) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U &ptr) {
            ptr.~U();
        });

        _index = Meta::indexOf<T, Ts...>();
        new (_buf) T(std::move(value));

        return *this;
    }

    always_inline Var &operator=(Var const &other) {
        *this = Var(other);
        return *this;
    }

    always_inline Var &operator=(Var &&other) {
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
    always_inline T &unwrap() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            panic("Unwrapping wrong type");
        }

        return *reinterpret_cast<T *>(_buf);
    }

    template <Meta::Contains<Ts...> T>
    always_inline T const &unwrap() const {
        if (_index != Meta::indexOf<T, Ts...>()) {
            panic("Unwrapping wrong type");
        }

        return *reinterpret_cast<T const *>(_buf);
    }

    template <typename T, typename... Args>
    always_inline T &emplace(Args &&...args) {
        if (_index != Meta::indexOf<T, Ts...>()) {
            Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U &ptr) {
                ptr.~U();
            });

            _index = Meta::indexOf<T, Ts...>();
        }

        return *new (_buf) T(std::forward<Args>(args)...);
    }

    template <Meta::Contains<Ts...> T>
    always_inline Opt<T> take() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            return NONE;
        }

        return std::move(*reinterpret_cast<T *>(_buf));
    }

    template <Meta::Contains<Ts...> T>
    always_inline Opt<T> take() const {
        if (_index != Meta::indexOf<T, Ts...>()) {
            return NONE;
        }

        return *reinterpret_cast<T const *>(_buf);
    }

    always_inline auto visit(auto visitor) {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    always_inline auto visit(auto visitor) const {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    always_inline static auto foreach(auto visitor) {
        return Meta::foreach<Ts...>(visitor);
    }

    template <Meta::Contains<Ts...> T>
    always_inline T const *is() const {
        return _index == Meta::indexOf<T, Ts...>() ? (T *)_buf : nullptr;
    }

    template <Meta::Contains<Ts...> T>
    always_inline T *is() {
        return _index == Meta::indexOf<T, Ts...>() ? (T *)_buf : nullptr;
    }

    always_inline usize index() const { return _index; }

    template <Meta::Contains<Ts...> T>
    std::partial_ordering operator<=>(T const &other) const {
        if constexpr (Meta::Comparable<T>)
            if (is<T>())
                return unwrap<T>() <=> other;
        return std::partial_ordering::unordered;
    }

    std::partial_ordering operator<=>(Var const &other) const {
        if (_index == other._index)
            return visit([&]<typename T>(T const &ptr) {
                if constexpr (Meta::Comparable<T>) {
                    return ptr <=> other.unwrap<T>();
                } else {
                    return std::partial_ordering::unordered;
                }
            });
        return std::partial_ordering::unordered;
    }
};

template <typename... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

} // namespace Karm
