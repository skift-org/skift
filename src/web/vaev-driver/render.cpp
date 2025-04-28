module;

#include <karm-base/box.h>
#include <karm-scene/base.h>
#include <karm-scene/clear.h>
#include <karm-sys/time.h>
#include <karm-text/book.h>
#include <vaev-style/computer.h>

export module Vaev.Driver:render;

import Vaev.Layout;
import :loader;

namespace Vaev::Driver {

export struct RenderResult {
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Frag> frag;
};

export RenderResult render(Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport) {
    Text::FontBook fontBook;
    if (not fontBook.loadAll())
        logWarn("not all fonts were properly loaded into fontbook");

    Style::Computer computer{media, *dom->styleSheets, fontBook};
    computer.loadFontFaces();

    Layout::Tree tree = {
        Layout::build(computer, dom),
        viewport
    };

    auto canvasColor = fixupBackgrounds(computer, dom, tree);

    auto [outDiscovery, root] = Layout::layoutCreateFragment(
        tree,
        {
            .knownSize = {viewport.small.width, NONE},
            .availableSpace = {viewport.small.width, 0_au},
            .containingBlock = {viewport.small.width, viewport.small.height},
        }
    );

    auto sceneRoot = makeRc<Scene::Stack>();

    Layout::paint(root, *sceneRoot);
    sceneRoot->prepare();

    return {
        makeRc<Layout::Box>(std::move(tree.root)),
        makeRc<Scene::Clear>(sceneRoot, canvasColor),
        makeRc<Layout::Frag>(std::move(root)),
    };
}

} // namespace Vaev::Driver
