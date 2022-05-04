#pragma once

#include "_prelude.h"

#include "std.h"

namespace Karm::Base {

template <typename T, size_t N>
struct Array {
    T _buf[N];

    constexpr Array() = default;

    constexpr size_t len() { return N; }

    constexpr T *begin() { return _buf; }

    constexpr T *end() { return _buf + N; }
};

} // namespace Karm::Base
