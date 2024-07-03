#include <vaev-dom/document.h>
#include <vaev-dom/element.h>
#include <vaev-dom/text.h>

#include "block.h"
#include "builder.h"
#include "flex.h"
#include "grid.h"
#include "image.h"
#include "inline.h"
#include "run.h"
#include "table.h"

namespace Vaev::Layout {

void buildChildren(Style::Computer &c, Vec<Strong<Dom::Node>> const &children, Flow &parent) {
    for (auto &child : children) {
        build(c, *child, parent);
    }
}

Strong<Flow> buildForDisplay(Display const &display, Strong<Style::Computed> style) {
    if (display.type() == Display::INTERNAL)
        return makeStrong<InlineFlow>(style);

    switch (display.inside()) {
    case Display::Inside::FLOW:
    case Display::Inside::FLOW_ROOT:
        return makeStrong<BlockFlow>(style);

    case Display::Inside::FLEX:
        return makeStrong<FlexFlow>(style);

    case Display::Inside::GRID:
        return makeStrong<GridFlow>(style);

    case Display::Inside::TABLE:
        return makeStrong<TableFlow>(style);

    default:
        return makeStrong<InlineFlow>(style);
    }
}

void buildElement(Style::Computer &c, Dom::Element const &el, Flow &parent) {
    auto style = c.computeFor(el);

    if (el.tagName == Html::IMG) {
        parent.add(makeStrong<ImageFrag>(style, Media::Image::fallback()));
        return;
    }

    auto display = style->display;

    if (display == Display::NONE)
        return;

    if (display == Display::CONTENTS) {
        buildChildren(c, el.children(), parent);
        return;
    }

    auto frag = buildForDisplay(display, style);
    buildChildren(c, el.children(), *frag);
    parent.add(frag);
}

void build(Style::Computer &c, Dom::Node const &node, Flow &parent) {
    if (auto *el = node.is<Dom::Element>()) {
        buildElement(c, *el, parent);
    } else if (auto *text = node.is<Dom::Text>()) {
        parent.add(makeStrong<Run>(parent._style, text->data));
    } else if (auto *doc = node.is<Dom::Document>()) {
        buildChildren(c, doc->children(), parent);
    }
}

Strong<Frag> build(Style::Computer &c, Dom::Document const &doc) {
    auto root = makeStrong<BlockFlow>(makeStrong<Style::Computed>());
    build(c, doc, *root);
    return root;
}

} // namespace Vaev::Layout
