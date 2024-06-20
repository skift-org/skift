#pragma once

#include <vaev-base/tags.h>

namespace Vaev::Svg {

enum struct TagId : u16 {
#define TAG(IDENT, _) IDENT,
#include <vaev-svg/defs/tag-names.inc>
#undef TAG
};

enum struct AttrId : u16 {
#define ATTR(IDENT, _) IDENT,
#include <vaev-svg/defs/attr-names.inc>
#undef ATTR
};

#define TAG(IDENT, _) \
    inline constexpr TagName IDENT = TagId::IDENT;
#include <vaev-svg/defs/tag-names.inc>
#undef TAG

} // namespace Vaev::Svg
