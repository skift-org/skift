#include <vaev-dom/element.h>
#include <vaev-dom/text.h>

#include "block.h"
#include "flex.h"
#include "grid.h"
#include "image.h"
#include "inline.h"
#include "mod.h"
#include "run.h"

namespace Vaev::Layout {

void build(Style::Computer &c, Vec<Strong<Dom::Node>> const &children, Flow &parent) {
    for (auto &child : children) {
        build(c, *child, parent);
    }
}

Strong<Flow> buildForDisplay(Display const &display, Strong<Style::Computed> style) {
    switch (display.inside()) {
    case Display::Inside::FLOW:
    case Display::Inside::FLOW_ROOT:
        return makeStrong<BlockFlow>(style);
    case Display::Inside::FLEX:
        return makeStrong<FlexFlow>(style);
    case Display::Inside::GRID:
        return makeStrong<GridFlow>(style);

    default:
        return makeStrong<InlineFlow>(style);
    }
}

void build(Style::Computer &c, Dom::Element const &el, Flow &parent) {
    auto style = c.computeFor(el);

    if (el.tagName == Html::IMG) {
        parent.add(makeStrong<ImageFrag>(style, Media::Image::fallback()));
        return;
    }

    auto display = style->display;

    if (display == Display::NONE)
        return;

    if (display == Display::CONTENT) {
        build(c, el.children(), parent);
        return;
    }

    auto frag = buildForDisplay(display, style);
    build(c, el.children(), *frag);
}

void build(Style::Computer &c, Dom::Node const &node, Flow &parent) {
    if (auto *el = node.is<Dom::Element>()) {
        build(c, *el, parent);
    } else if (auto *text = node.is<Dom::Text>()) {
        parent.add(makeStrong<Run>(parent._style, text->data));
    }
}

} // namespace Vaev::Layout
