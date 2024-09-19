#include <karm-sys/time.h>
#include <vaev-layout/frag.h>
#include <vaev-markup/dom.h>
#include <vaev-paint/page.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computer.h>

#include "fetcher.h"
#include "render.h"

namespace Vaev::Driver {

static constexpr bool DEBUG_RENDER = false;

static void _collectStyle(Markup::Node const &node, Style::StyleBook &sb) {
    auto *el = node.is<Markup::Element>();
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

RenderResult render(Markup::Document const &dom, Style::Media const &media, Vec2Px viewport) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-view/user-agent.css"_url)
            .take("user agent stylesheet not available")
    );

    auto start = Sys::now();
    _collectStyle(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "style collection time: {}", elapsed);

    start = Sys::now();

    Style::Computer computer{media, stylebook};
    Layout::Tree tree = {
        Layout::build(computer, dom),
        {
            .small = viewport,
        }
    };

    elapsed = Sys::now() - start;

    logDebugIf(DEBUG_RENDER, "layout tree build time: {}", elapsed);

    start = Sys::now();

    Layout::Viewport vp{.small = viewport};

    elapsed = Sys::now() - start;

    logDebugIf(DEBUG_RENDER, "layout tree measure time: {}", elapsed);

    start = Sys::now();

    Layout::layout(
        tree,
        tree.root,
        {
            .commit = Layout::Commit::YES,
            .knownSize = {vp.small.width, NONE},
            .availableSpace = {vp.small.width, Px{0}},
            .containingBlock = {vp.small.width, vp.small.height},
        }
    );

    auto paintRoot = makeStrong<Paint::Stack>();

    elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "layout tree layout time: {}", elapsed);

    auto paintStart = Sys::now();

    Layout::paint(tree.root, *paintRoot);
    paintRoot->prepare();

    elapsed = Sys::now() - paintStart;
    logDebugIf(DEBUG_RENDER, "layout tree paint time: {}", elapsed);

    return {
        std::move(stylebook),
        makeStrong<Layout::Frag>(std::move(tree.root)),
        paintRoot,
    };
}

RenderResult render(Markup::Document &dom, Style::Media const &media, Print::PaperStock paper) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-view/user-agent.css"_url)
            .take("user agent stylesheet not available")
    );

    auto start = Sys::now();
    _collectStyle(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "style collection time: {}", elapsed);

    Style::Computer computer{media, stylebook};

    Layout::Viewport vp{
        .small = {
            Px{paper.width},
            Px{paper.height},
        },
    };

    Layout::Tree tree = {
        Layout::build(computer, dom),
        vp,
    };

    Layout::layout(
        tree,
        tree.root,
        {
            .commit = Layout::Commit::YES,
            .knownSize = {vp.small.width, NONE},
            .availableSpace = {vp.small.width, Px{0}},
            .containingBlock = {vp.small.width, vp.small.height},
        }
    );

    auto paintRoot = makeStrong<Paint::Page>();
    Layout::paint(tree.root, *paintRoot);
    paintRoot->prepare();

    return {
        std::move(stylebook),
        makeStrong<Layout::Frag>(std::move(tree.root)),
        paintRoot,
    };
}

} // namespace Vaev::Driver
