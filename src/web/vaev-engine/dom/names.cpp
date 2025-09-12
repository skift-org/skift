export module Vaev.Engine:dom.names;

import Karm.Core;

using namespace Karm;

namespace Vaev {

namespace Dom {
// https://dom.spec.whatwg.org/#concept-element-qualified-name
// https://dom.spec.whatwg.org/#concept-attribute-qualified-name
export struct QualifiedName {
    Symbol ns; // https://www.w3.org/TR/2011/WD-html5-20110525/namespaces.html
    Symbol name;

    bool operator==(QualifiedName const& other) const = default;

    void repr(Io::Emit& e) const;
};

} // namespace Dom

namespace Html {

export Symbol NAMESPACE = "http://www.w3.org/1999/xhtml"_sym;

#define TAG(IDENT, VALUE) export Dom::QualifiedName IDENT##_TAG = {NAMESPACE, Symbol::from(#VALUE)};
#include "defs/ns-html-tag-names.inc"
#undef TAG

export Array const VOID_TAGS = {
    AREA_TAG,
    BASE_TAG,
    BR_TAG,
    COL_TAG,
    EMBED_TAG,
    HR_TAG,
    IMG_TAG,
    INPUT_TAG,
    LINK_TAG,
    META_TAG,
    PARAM_TAG,
    SOURCE_TAG,
    TRACK_TAG,
    WBR_TAG
};

// FIXME: remaining elements from https://html.spec.whatwg.org/#has-an-element-in-scope
export Array const BASIC_SCOPE_DELIMITERS = {
    APPLET_TAG,
    CAPTION_TAG,
    HTML_TAG,
    TABLE_TAG,
    TD_TAG,
    TH_TAG,
    MARQUEE_TAG,
    OBJECT_TAG,
    TEMPLATE_TAG
};

export Array const IMPLIED_END_TAGS = {
    DD_TAG,
    DT_TAG,
    LI_TAG,
    OPTION_TAG,
    OPTGROUP_TAG,
    P_TAG,
    RB_TAG,
    RP_TAG,
    RT_TAG,
    RT_TAG
};

#define ATTR(IDENT, VALUE) export Dom::QualifiedName IDENT##_ATTR = {NAMESPACE, Symbol::from(#VALUE)};
#include "defs/ns-html-attr-names.inc"
#undef ATTR

} // namespace Html

namespace Svg {

export Symbol NAMESPACE = "http://www.w3.org/2000/svg"_sym;

#define TAG(IDENT, VALUE) export Dom::QualifiedName IDENT##_TAG = {NAMESPACE, Symbol::from(#VALUE)};
#include "defs/ns-svg-tag-names.inc"
#undef TAG

#define ATTR(IDENT, VALUE) export Dom::QualifiedName IDENT##_ATTR = {NAMESPACE, Symbol::from(#VALUE)};
#include "defs/ns-svg-attr-names.inc"
#undef ATTR

export Symbol qualifiedAttrNameCased(Str name) {
#define ATTR(IDENT, VALUE)       \
    if (eqCi(Str(#VALUE), name)) \
        return Symbol::from(Str(#VALUE));
#include "defs/ns-svg-attr-names.inc"
#undef ATTR
    return Symbol::from(name);
}

export Symbol qualifiedTagNameCased(Str name) {
#define TAG(IDENT, VALUE)        \
    if (eqCi(Str(#VALUE), name)) \
        return Symbol::from(Str(#VALUE));
#include "defs/ns-svg-tag-names.inc"
#undef TAG
    return Symbol::from(name);
}

} // namespace Svg

namespace MathMl {

export Symbol NAMESPACE = "http://www.w3.org/1998/Math/MathML"_sym;

#define TAG(IDENT, VALUE) export Dom::QualifiedName IDENT##_TAG = {NAMESPACE, Symbol::from(#VALUE)};
#include "defs/ns-mathml-tag-names.inc"
#undef TAG

#define ATTR(IDENT, VALUE) export Dom::QualifiedName IDENT##_ATTR = {NAMESPACE, Symbol::from(#VALUE)};
#include "defs/ns-mathml-attr-names.inc"
#undef ATTR

} // namespace MathMl

namespace Dom {

void Dom::QualifiedName::repr(Io::Emit& e) const {
    Str displayNamespace = ns.str();
    if (ns == Html::NAMESPACE) {
        displayNamespace = "html";
    } else if (ns == Svg::NAMESPACE) {
        displayNamespace = "svg";
    } else if (ns == MathMl::NAMESPACE) {
        displayNamespace = "mathml";
    }
    e("{}:{}", displayNamespace, name);
}

} // namespace Dom

} // namespace Vaev
