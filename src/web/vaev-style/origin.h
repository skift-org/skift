#pragma once

#include <karm-base/base.h>

namespace Vaev::Style {

// https://drafts.csswg.org/css-cascade/#origin
enum struct Origin {
    // NOTE: The order of these values is important
    USER_AGENT,
    USER,
    AUTHOR,
};

static inline std::strong_ordering operator<=>(Origin a, Origin b) {
    return static_cast<u8>(a) <=> static_cast<u8>(b);
}

static inline bool operator==(Origin a, Origin b) {
    return static_cast<u8>(a) == static_cast<u8>(b);
}

} // namespace Vaev::Style
