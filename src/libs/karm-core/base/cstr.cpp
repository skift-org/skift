module;

#include <karm-core/macros.h>

export module Karm.Core:base.cstr;

import :base.base;

namespace Karm {

#pragma clang unsafe_buffer_usage begin

export using CStr = char const*;

export always_inline constexpr usize cstrLen(CStr s) {
    usize len = 0;
    while (*s++) {
        len++;
    }
    return len;
}

export always_inline constexpr bool cstrEq(char const* str1, char const* str2) {
    while (*str1 and *str2) {
        if (*str1++ != *str2++)
            return false;
    }
    return *str1 == *str2;
}

#pragma clang unsafe_buffer_usage end

} // namespace Karm
