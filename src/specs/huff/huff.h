#pragma once

#include <karm-base/array.h>

namespace Huff {

template <typename T>
struct Tree {
    Array<T, sizeof(T) * 8> len;
    Array<T, sizeof(T) * 8> sym;
};

} // namespace Huff
