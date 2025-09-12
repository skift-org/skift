module;

#include <karm-gfx/canvas.h>
#include <karm-math/au.h>

export module Vaev.View:view;

import Karm.Gc;
import Karm.Print;
import Karm.Ui;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::View {

export struct ViewProps {
    bool wireframe = false;
    Gc::Ptr<Dom::Node> selected = nullptr;
};

struct View : Ui::View<View> {
    Gc::Root<Dom::Document> _dom;
    ViewProps _props;
    Opt<Driver::RenderResult> _renderResult;

    View(Gc::Root<Dom::Document> dom, ViewProps props)
        : _dom(dom), _props(props) {}

    void reconcile(View& o) override {
        _dom = o._dom;
        _props = o._props;
        _renderResult = NONE;
    }

    void paint(Gfx::Canvas& g, Math::Recti rect) override {
        // Painting browser's viewport.
        auto viewport = bound().size();
        if (not _renderResult) {
            auto media = Style::Media::forView(viewport, Ui::darkMode ? ColorScheme::DARK : ColorScheme::LIGHT);
            _renderResult = Driver::render(*_dom, media, {.small = viewport.cast<Au>()});
        }

        g.push();

        g.origin(bound().xy.cast<f64>());
        g.clip(viewport);

        auto [layout, paint, frag] = *_renderResult;
        auto paintRect = rect.offset(-bound().xy);
        paint->paint(g, paintRect.cast<f64>());

        if (_props.wireframe)
            Layout::wireframe(*frag, g);

        if (_props.selected)
            Layout::overlay(*frag, g, _props.selected.upgrade());

        g.pop();
    }

    void layout(Math::Recti bound) override {
        _renderResult = NONE;
        Ui::View<View>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i size, Ui::Hint) override {
        // FIXME: This is wasteful, we should cache the result
        auto media = Style::Media::forView(size, Ui::darkMode ? ColorScheme::DARK : ColorScheme::LIGHT);
        auto [layout, _, frag] = Driver::render(*_dom, media, {.small = size.cast<Au>()});

        return {
            frag->metrics.borderBox().width.cast<isize>(),
            frag->metrics.borderBox().height.cast<isize>(),
        };
    }
};

export Ui::Child view(Gc::Root<Dom::Document> dom, ViewProps props) {
    return makeRc<View>(dom, props);
}

} // namespace Vaev::View
