#pragma once

#include "_prelude.h"

#include "std.h"

namespace Karm::Base {

template <typename T, size_t N>
struct Array {
    T _buf[N];

    constexpr Array() = default;

    constexpr auto len() -> size_t { return N; }

    constexpr auto begin() -> T * { return _buf; }

    constexpr auto end() -> T * { return _buf + N; }
};

} // namespace Karm::Base
