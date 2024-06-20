#include <vaev-dom/element.h>
#include <vaev-dom/text.h>

#include "block.h"
#include "image.h"
#include "inline.h"
#include "mod.h"
#include "run.h"

namespace Vaev::Layout {

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

void build(Dom::Element const &el, Vec<Strong<Frag>> &out) {
    if (el.tagName == Html::IMG)
        out.pushBack(makeStrong<ImageFrag>(makeStrong<Style::Computed>(), Media::Image::fallback()));
    else if (contains(BLOCKS, el.tagName)) {
        auto frag = makeStrong<BlockFlow>(makeStrong<Style::Computed>());
        build(el.children(), frag->_frags);
        out.pushBack(frag);
    } else {
        auto frag = makeStrong<InlineFlow>(makeStrong<Style::Computed>());
        build(el.children(), frag->_frags);
        out.pushBack(frag);
    }
}

void build(Dom::Node const &node, Vec<Strong<Frag>> &out) {
    if (auto *el = node.is<Dom::Element>()) {
        build(*el, out);
    } else if (auto *text = node.is<Dom::Text>()) {
        out.pushBack(makeStrong<Run>(makeStrong<Style::Computed>(), text->data));
    }
}

} // namespace Vaev::Layout
