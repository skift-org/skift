#include <web-dom/element.h>
#include <web-dom/text.h>

#include "block.h"
#include "image.h"
#include "inline.h"
#include "mod.h"
#include "text.h"

namespace Web::Layout {

// FIXME: For now this is hadcoded, but it should follow the display property.
static Array BLOCKS = {
    Html::DIV,
    Html::P,
    Html::H1,
    Html::H2,
    Html::H3,
    Html::H4,
    Html::H5,
    Html::H6,
};

void build(Vec<Strong<Dom::Node>> const &children, Vec<Strong<Frag>> &out) {
    for (auto &child : children) {
        build(*child, out);
    }
}

void build(Dom::Element const &n, Vec<Strong<Frag>> &out) {
    if (n.tagName == Html::IMG)
        out.pushBack(makeStrong<ImageFrag>(makeStrong<Css::ComputedValues>(), Media::Image::fallback()));
    else if (contains(BLOCKS, n.tagName)) {
        auto frag = makeStrong<BlockFlow>(makeStrong<Css::ComputedValues>());
        build(n.children(), frag->children);
        out.pushBack(frag);
    } else {
        auto frag = makeStrong<InlineFlow>(makeStrong<Css::ComputedValues>());
        build(n.children(), frag->children);
        out.pushBack(frag);
    }
}

void build(Dom::Node const &n, Vec<Strong<Frag>> &out) {
    if (auto *el = n.is<Dom::Element>()) {
        build(*el, out);
    } else if (auto *text = n.is<Dom::Text>()) {
        out.pushBack(makeStrong<TextFrag>(makeStrong<Css::ComputedValues>(), text->data));
    }
}

} // namespace Web::Layout
