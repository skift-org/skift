#pragma once

#include "_prelude.h"

#include "std.h"

namespace Karm::Base {

template <typename T, size_t N>
struct Array {
    T _buf[N];

    constexpr Array() = default;

    constexpr size_t len() { return N; }

    constexpr auto begin() { return _buf; }

    constexpr auto end() { return _buf + N; }
};

} // namespace Karm::Base
