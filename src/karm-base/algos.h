#pragma once

#include "std.h"

namespace Karm {

void reverse(auto slice) {
    for (auto i = 0u; i < slice.len() / 2; i++) {
        std::swap(slice[i], slice[slice.len() - i - 1]);
    }
}

} // namespace Karm
