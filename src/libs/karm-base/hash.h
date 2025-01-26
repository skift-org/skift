#pragma once

#include "checked.h"
#include "slice.h"

namespace Karm {

using Hash = usize;

template <typename T>
struct Hasher;

template <typename T>
concept Hashable = requires(T t) {
    { Hasher<T>::hash(t) } -> Meta::Same<Hash>;
};

template <Hashable T>
Hash hash(T const& v) {
    return Hasher<T>::hash(v);
}

template <>
struct Hasher<Hash> {
    static constexpr Hash hash(Hash h) {
        return h;
    }
};

template <>
struct Hasher<Bytes> {
    static constexpr Hash hash(Bytes bytes) {
        Hash hash = 0;
        for (auto& b : bytes)
            hash = (1000003 * hash) ^ b;
        hash ^= bytes.len();
        return hash;
    }
};

template <Sliceable T>
struct Hasher<T> {
    static constexpr Hash hash(T const& v) {
        Hash hash{0};
        for (auto& e : v)
            hash = hash + ::hash(e);
        return hash;
    }
};

template <Meta::Integral T>
struct Hasher<T> {
    static constexpr Hash hash(T const& v) {
        return Hasher<Bytes>::hash({reinterpret_cast<Byte const*>(&v), sizeof(v)});
    }
};

template <Meta::Float T>
struct Hasher<T> {
    static constexpr Hash hash(T const& v) {
        return Hasher<Bytes>::hash({reinterpret_cast<Byte const*>(&v), sizeof(v)});
    }
};

} // namespace Karm
