#include <karm-sys/time.h>
#include <vaev-dom/element.h>
#include <vaev-layout/builder.h>
#include <vaev-layout/sizing.h>
#include <vaev-paint/page.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computer.h>

#include "fetcher.h"
#include "render.h"

namespace Vaev::Driver {

static void _collectStyle(Dom::Node const &node, Style::StyleBook &sb) {
    auto *el = node.is<Dom::Element>();
    if (el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Style::StyleSheet::parse(textScan);
        sb.add(std::move(sheet));
    } else if (el and el->tagName == Html::LINK) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                return;
            }

            auto url = Mime::parseUrlOrPath(*href);
            if (not url) {
                logWarn("link element href attribute is not a valid URL: {}", *href);
                return;
            }

            auto sheet = fetchStylesheet(url.take());
            if (not sheet) {
                logWarn("failed to fetch stylesheet: {}", sheet);
                return;
            }

            sb.add(sheet.take());
        }
    } else {
        for (auto &child : node.children())
            _collectStyle(*child, sb);
    }
}

RenderResult render(Dom::Document const &dom, Style::Media const &media, Vec2Px viewport) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-view/user-agent.css"_url)
            .take("user agent stylesheet not available")
    );

    auto start = Sys::now();
    _collectStyle(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebug("style collection time: {}", elapsed);

    Style::Computer computer{media, stylebook};
    auto layoutRoot = Layout::build(computer, dom);

    Layout::Viewport vp{.small = viewport};
    Layout::Context ctx{
        .viewport = vp,
        .frag = *layoutRoot,
        .axis = Axis::VERTICAL,
        .containingBlock = vp.small,
        .font = Text::Font::fallback(),
    };

    auto height = Layout::computePreferredOuterSize(ctx, Axis::VERTICAL);

    Layout::Box containingBox = {
        .borderBox = {vp.small.width, height},
    };

    layoutRoot->placeChildren(ctx, containingBox);

    auto paintRoot = makeStrong<Paint::Stack>();
    layoutRoot->makePaintables(*paintRoot);
    paintRoot->prepare();

    return {layoutRoot, paintRoot};
}

RenderResult render(Dom::Document &dom, Style::Media const &media, Print::PaperStock paper) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-view/user-agent.css"_url)
            .take("user agent stylesheet not available")
    );

    auto start = Sys::now();
    _collectStyle(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebug("style collection time: {}", elapsed);

    Style::Computer computer{media, stylebook};
    auto layoutRoot = Layout::build(computer, dom);

    Layout::Viewport vp{
        .small = {
            Px{paper.width},
            Px{paper.height},
        },
    };

    Layout::Context ctx{
        .viewport = vp,
        .frag = *layoutRoot,
        .axis = Axis::VERTICAL,
        .containingBlock = vp.small,
        .font = Text::Font::fallback(),
    };

    auto height = Layout::computePreferredOuterSize(ctx, Axis::VERTICAL);

    Layout::Box containingBox = {
        .borderBox = {vp.small.width, height},
    };

    layoutRoot->placeChildren(ctx, containingBox);

    auto paintRoot = makeStrong<Paint::Page>();
    layoutRoot->makePaintables(*paintRoot);
    paintRoot->prepare();

    return {layoutRoot, paintRoot};
}

} // namespace Vaev::Driver
