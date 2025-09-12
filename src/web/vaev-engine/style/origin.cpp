export module Vaev.Engine:style.origin;

import Karm.Core;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-cascade/#origin
export enum struct Origin : u8 {
    // NOTE: The order of these values is important
    USER_AGENT,
    USER,
    AUTHOR,
    INLINE, //< Declarations from style attributes
};

export std::strong_ordering operator<=>(Origin a, Origin b) {
    return toUnderlyingType(a) <=> toUnderlyingType(b);
}

export bool operator==(Origin a, Origin b) {
    return toUnderlyingType(a) == toUnderlyingType(b);
}

} // namespace Vaev::Style
