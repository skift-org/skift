#pragma once

#include <vaev-base/tags.h>

namespace Vaev::MathMl {

enum struct TagId : u16 {
#define TAG(IDENT, _) IDENT,
#include "defs/tag-names.inc"
#undef TAG
};

enum struct AttrId : u16 {
#define ATTR(IDENT, _) IDENT,
#include "defs/attr-names.inc"
#undef ATTR
};

#define TAG(IDENT, _) \
    inline constexpr TagName IDENT = TagId::IDENT;
#include "defs/tag-names.inc"
#undef TAG

} // namespace Vaev::MathMl
