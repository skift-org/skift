#pragma once

#include "std.h"

namespace Karm {

using CStr = char const *;

inline constexpr usize cstrLen(CStr s) {
    usize len = 0;
    while (*s++) {
        len++;
    }
    return len;
}

inline bool cstrEq(char const *str1, char const *str2) {
    while (*str1 and *str2) {
        if (*str1++ != *str2++)
            return false;
    }
    return *str1 == *str2;
}

} // namespace Karm
