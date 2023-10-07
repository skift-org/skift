#pragma once

#include <karm-base/checked.h>
#include <karm-base/slice.h>

namespace Karm {

struct Hash {
    usize _value;

    constexpr explicit operator usize() const {
        return _value;
    }
    constexpr auto operator<=>(Hash const &) const = default;
    constexpr auto operator+(Hash const &o) const {
        return Hash(_value + o._value + 0x9e3779b9 + (_value << 6) + (_value >> 2));
    }
};

template <typename T>
struct Hasher;

template <>
struct Hasher<Hash> {
    static constexpr Hash hash(Hash h) {
        return h;
    }
};

template <>
struct Hasher<Bytes> {
    static constexpr Hash hash(Bytes bytes) {
        usize hash = 0;
        for (auto &b : bytes)
            hash = (1000003 * hash) ^ b;
        hash ^= bytes.len();
        return {hash};
    }
};

template <Sliceable T>
struct Hasher<T> {
    static constexpr Hash hash(T const &v) {
        Hash hash{0};
        for (auto &e : v)
            hash = hash + Hasher<decltype(e)>::hash(e);
        return hash;
    }
};

template <Meta::Integral T>
struct Hasher<T> {
    static constexpr Hash hash(T const &v) {
        return Hasher<Bytes>::hash({reinterpret_cast<Byte const *>(&v), sizeof(v)});
    }
};

template <Meta::Float T>
struct Hasher<T> {
    static constexpr Hash hash(T const &v) {
        return Hasher<Bytes>::hash({reinterpret_cast<Byte const *>(&v), sizeof(v)});
    }
};

template <typename T>
concept Hashable = requires(T t) {
    { Hasher<T>::hash(t) } -> Meta::Same<Hash>;
};

template <Hashable T>
Hash hash(T const &v) {
    return Hasher<T>::hash(v);
}

} // namespace Karm
