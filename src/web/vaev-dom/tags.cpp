#include "tags.h"

namespace Vaev::Html {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/ns-html-tag-names.inc"
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
#include "defs/ns-html-attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/ns-html-tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/ns-html-attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vaev::Html

namespace Vaev::MathMl {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/ns-mathml-tag-names.inc"
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
#include "defs/ns-mathml-attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/ns-mathml-tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/ns-mathml-attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vaev::MathMl

namespace Vaev::Svg {

Str _tagName(TagId id) {
    switch (id) {
#define TAG(IDENT, NAME) \
    case TagId::IDENT:   \
        return #NAME;
#include "defs/ns-svg-tag-names.inc"
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
#include "defs/ns-svg-attr-names.inc"
#undef ATTR
    default:
        return "unknown";
    }
}

Opt<TagId> _tagId(Str name) {
#define TAG(IDENT, NAME) \
    if (name == #NAME)   \
        return TagId::IDENT;
#include "defs/ns-svg-tag-names.inc"
#undef TAG

    return NONE;
}

Opt<AttrId> _attrId(Str name) {
#define ATTR(IDENT, NAME) \
    if (name == #NAME)    \
        return AttrId::IDENT;

#include "defs/ns-svg-attr-names.inc"
#undef ATTR

    return NONE;
}

} // namespace Vaev::Svg
