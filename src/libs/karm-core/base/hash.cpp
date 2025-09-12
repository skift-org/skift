export module Karm.Core:base.hash;

import :meta.traits;

import :base.base;

namespace Karm {

// https://www.ietf.org/archive/id/draft-eastlake-fnv-21.html
export constexpr u64 fnv64(u8 const* buf, usize len) {
    u64 hash = 0xcbf29ce484222325;
    for (usize i = 0; i < len; i++) {
        hash ^= buf[i];
        hash *= 0x100000001b3;
    }
    return hash;
}

export constexpr u64 hash(Meta::Boolean auto const& v) {
    return hash(v ? 0x1 : 0x0);
}

static_assert(Meta::Integral<char>);

export constexpr u64 hash(Meta::Integral auto const& v) {
    return fnv64(reinterpret_cast<u8 const*>(&v), sizeof(v));
}

export constexpr u64 hash(Meta::Float auto const& v) {
    return fnv64(reinterpret_cast<u8 const*>(&v), sizeof(v));
}

export template <typename T>
constexpr u64 hash(T const& t)
    requires requires(T const t) {
        { t.hash() } -> Meta::Same<u64>;
    }
{
    return t.hash();
}

export constexpr u64 hash(Meta::Enum auto const& v) {
    return hash(toUnderlyingType(v));
}

export constexpr u64 hash() {
    return 0xcbf29ce484222325;
}

export template <typename T>
constexpr u64 hash(u64 a, T const& v) {
    return a ^ (hash(v) + 0x9e3779b97f4a7c15);
}

} // namespace Karm
