#pragma once

#include <karm-base/base.h>

#include "traits.h"

namespace Karm::Meta {

using Id = usize;

template <typename T>
static constexpr Id idOf() {
    char const* cstr = __PRETTY_FUNCTION__;
    usize len = sizeof(__PRETTY_FUNCTION__);
    usize hash = 0uz;
    for (char const* b = cstr; b < cstr + len; b++)
        hash = (1000003 * hash) ^ *b;
    hash ^= len;
    return hash;
}

} // namespace Karm::Meta
