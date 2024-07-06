#include <karm-sys/time.h>
#include <vaev-css/parse.h>
#include <vaev-dom/element.h>
#include <vaev-layout/builder.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computer.h>

#include "render.h"

namespace Vaev::View {

static void _collectStyle(Dom::Node const &node, Style::StyleBook &sb) {
    auto *el = node.is<Dom::Element>();
    if (el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Css::parseStylesheet(textScan);
        sb.add(std::move(sheet));
    } else if (el and el->tagName == Html::LINK) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href)
                return;
            auto url = Mime::parseUrlOrPath(*href);
            if (not url)
                return;
            auto sheet = Css::fetchStylesheet(url.take());
            if (not sheet)
                return;
            sb.add(sheet.take());
        }
    } else {
        for (auto &child : node.children())
            _collectStyle(*child, sb);
    }
}

RenderResult render(Dom::Document const &dom, Style::Media const &media, Vec2Px viewport) {
    Style::StyleBook stylebook;
    stylebook.add(Css::fetchStylesheet("bundle://vaev-view/user-agent.css"_url).take("user agent stylesheet not available"));

    auto start = Sys::now();
    _collectStyle(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebug("style collection time: {}ms", elapsed.toUSecs() / 1000.0);

    Style::Computer computer{media, stylebook};
    auto layoutRoot = Layout::build(computer, dom);
    layoutRoot->layout(viewport);

    auto paintRoot = makeStrong<Paint::Stack>();
    layoutRoot->paint(*paintRoot);
    paintRoot->prepare();

    return {layoutRoot, paintRoot};
}

} // namespace Vaev::View
