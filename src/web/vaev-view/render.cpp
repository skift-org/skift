#include <karm-sys/time.h>
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

Cons<Strong<Layout::Frag>, Strong<Paint::Node>> render(Dom::Document const &dom, Vec2Px viewport) {
    Style::StyleBook stylebook;
    stylebook.add(Css::fetchStylesheet("bundle://vaev-view/user-agent.css"_url).take());
    _collectStyle(dom, stylebook);

    Style::Computer computer{stylebook};
    Strong<Layout::Flow> layoutRoot = makeStrong<Layout::BlockFlow>(makeStrong<Style::Computed>());
    Layout::build(computer, dom, *layoutRoot);

    layoutRoot->layout(viewport);

    auto paintRoot = makeStrong<Paint::Stack>();
    layoutRoot->paint(*paintRoot);
    paintRoot->prepare();

    return {layoutRoot, paintRoot};
}

} // namespace Vaev::View
