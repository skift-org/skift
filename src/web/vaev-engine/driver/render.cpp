module;

#include <karm-gfx/buffer.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>

export module Vaev.Engine:driver.render;

import Karm.Gc;
import Karm.Scene;
import Karm.Font;
import :layout;
import :style;
import :dom;
import :values;

namespace Vaev::Driver {

export struct RenderResult {
    Rc<Layout::Box> layout;
    Rc<Scene::Node> scenes;
    Rc<Layout::Frag> frag;
};

export RenderResult render(Gc::Ref<Dom::Document> dom, Style::Media const& media, Layout::Viewport viewport) {
    Font::Database db;
    if (not db.loadSystemFonts())
        logWarn("not all fonts were properly loaded into database");

    Style::Computer computer{media, *dom->styleSheets, db};
    computer.build();
    computer.styleDocument(*dom);

    Layout::Tree tree = {
        Layout::build(dom),
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

export Rc<Gfx::Surface> renderToSurface(Gc::Ref<Dom::Document> dom, Vec2Au imageSize, Resolution scale) {
    auto media = Style::Media::forRender(imageSize, scale);
    Vec2Au viewportSize = {media.width, media.height};
    auto [layout, scene, frags] = Vaev::Driver::render(*dom, media, {.small = viewportSize});
    return scene->snapshot(imageSize.cast<f64>(), scale.toDppx());
}

} // namespace Vaev::Driver
