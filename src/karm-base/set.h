#pragma once

#include "list.h"
#include "vec.h"

namespace Karm {

template <typename S>
struct _Set {
    using T = typename S::T;

    S _store;

    _Set() = default;

    _Set(std::initializer_list<T> &&other) : _store(std::move(other)) {}
};

template <typename T>
using Set = _Set<Vec<T>>;

template <typename T, size_t N>
using InlineSet = _Set<InlineVec<T, N>>;

template <typename T>
using ListSet = _Set<List<T>>;

} // namespace Karm
