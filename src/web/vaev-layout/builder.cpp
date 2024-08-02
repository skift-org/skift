#include <karm-text/loader.h>
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
    auto style = c.computeFor(parent.style(), el);

    if (el.tagName == Html::IMG) {
        Image::Picture img = Gfx::Surface::fallback();
        parent.add(makeStrong<ImageFrag>(style, img));
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

static Opt<Strong<Text::Fontface>> _regularFontface = NONE;

Strong<Text::Fontface> regularFontface() {
    if (not _regularFontface) {
        _regularFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
    }
    return *_regularFontface;
}

void buildRun(Style::Computer &, Dom::Text const &node, Flow &parent) {
    auto style = makeStrong<Style::Computed>(parent.style());
    auto font = Text::Font{regularFontface(), 16};
    Io::SScan scan{node.data};
    scan.eat(Re::space());
    if (scan.ended())
        return;
    auto run = makeStrong<Text::Run>(font);
    while (not scan.ended()) {
        run->append(scan.next());
        if (scan.eat(Re::space())) {
            run->append(' ');
        }
    }

    parent.add(makeStrong<Run>(style, run));
}

void build(Style::Computer &c, Dom::Node const &node, Flow &parent) {
    if (auto *el = node.is<Dom::Element>()) {
        buildElement(c, *el, parent);
    } else if (auto *text = node.is<Dom::Text>()) {
        buildRun(c, *text, parent);
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
