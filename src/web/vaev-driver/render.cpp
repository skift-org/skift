module;

#include <karm-base/box.h>
#include <karm-scene/stack.h>
#include <karm-sys/time.h>
#include <karm-text/book.h>
#include <vaev-style/computer.h>

export module Vaev.Driver:render;

import :loader;
import Vaev.Layout;

namespace Vaev::Driver {

static constexpr bool DEBUG_RENDER = false;

export struct RenderResult {
    Style::StyleBook style;
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Frag> frag;
};

export RenderResult render(Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport) {
    Style::StyleBook stylebook;
    stylebook.add(
        fetchStylesheet("bundle://vaev-driver/html.css"_url, Style::Origin::USER_AGENT)
            .take("user agent stylesheet not available")
    );

    auto start = Sys::now();
    fetchStylesheets(dom, stylebook);
    auto elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "style collection time: {}", elapsed);

    start = Sys::now();

    Text::FontBook fontBook;
    if (not fontBook.loadAll())
        logWarn("not all fonts were properly loaded into fontbook");

    Style::Computer computer{media, stylebook, fontBook};
    computer.loadFontFaces();

    Layout::Tree tree = {
        Layout::build(computer, dom),
        viewport,
    };

    elapsed = Sys::now() - start;

    logDebugIf(DEBUG_RENDER, "layout tree build time: {}", elapsed);

    start = Sys::now();

    auto [outDiscovery, root] = Layout::layoutCreateFragment(
        tree,
        {
            .knownSize = {viewport.small.width, NONE},
            .availableSpace = {viewport.small.width, 0_au},
            .containingBlock = {viewport.small.width, viewport.small.height},
        }
    );

    auto sceneRoot = makeRc<Scene::Stack>();

    elapsed = Sys::now() - start;
    logDebugIf(DEBUG_RENDER, "layout tree layout time: {}", elapsed);

    auto paintStart = Sys::now();
    Layout::paint(root, *sceneRoot);
    sceneRoot->prepare();

    elapsed = Sys::now() - paintStart;
    logDebugIf(DEBUG_RENDER, "layout tree paint time: {}", elapsed);

    return {
        std::move(stylebook),
        makeRc<Layout::Box>(std::move(tree.root)),
        sceneRoot,
        makeRc<Layout::Frag>(std::move(root))
    };
}

} // namespace Vaev::Driver
