#pragma once

#include <karm-base/clamp.h>
#include <karm-base/std.h>

namespace Karm::Text {

constexpr size_t u8strlen(char8_t const *str) {
    size_t len = 0;
    while (*str) {
        len++;
        str++;
    }
    return len;
}

constexpr int u8strcmp(char8_t const *lhs, size_t lhs_len, char8_t const *rhs, size_t rhs_len) {
    size_t len = Base::min(lhs_len, rhs_len);
    for (size_t i = 0; i < len; i++) {
        int cmp = lhs[i] - rhs[i];
        if (cmp != 0) {
            return cmp;
        }
    }
    return lhs_len - rhs_len;
}

} // namespace Karm::Text
