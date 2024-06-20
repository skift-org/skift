#include "tags.h"

namespace Vaev::MathMl {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/tag-names.inc"
#undef TAG
    default:
        return "unknown";
    }
}

Str _attrName(AttrId id) {
    switch (id) {
#define ATTR(IDENT, NAME) \
    case AttrId::IDENT:   \
        return #NAME;
#include "defs/attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vaev::MathMl
