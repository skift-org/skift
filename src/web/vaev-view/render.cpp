#include <vaev-css/mod.h>
#include <vaev-dom/element.h>
#include <vaev-layout/builder.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computer.h>

#include "render.h"

namespace Vaev::View {

static void _collectStyle(Dom::Node const &node, Style::StyleBook &sb) {
    if (auto *el = node.is<Dom::Element>(); el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Css::parseStylesheet(textScan);
        sb.add(std::move(sheet));
    } else {
        for (auto &child : node.children())
            _collectStyle(*child, sb);
    }
}

Strong<Paint::Node> render(Dom::Document const &dom, RectPx viewport) {
    logDebug("collecting styles");
    Style::StyleBook stylebook;
    _collectStyle(dom, stylebook);

    logDebug("building layout tree");
    Style::Computer computer{stylebook};
    Strong<Layout::Flow> layoutRoot = makeStrong<Layout::BlockFlow>(makeStrong<Style::Computed>());
    Layout::build(computer, dom, *layoutRoot);

    logDebug("computing layout");
    layoutRoot->layout(viewport);

    logDebug("painting layout");
    auto paintRoot = makeStrong<Paint::Stack>();
    layoutRoot->paint(*paintRoot);
    paintRoot->prepare();

    return paintRoot;
}

} // namespace Vaev::View
