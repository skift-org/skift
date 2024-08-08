#include <karm-ui/view.h>
#include <vaev-driver/render.h>
#include <vaev-layout/sizing.h>

#include "view.h"

namespace Vaev::View {

struct View : public Ui::View<View> {
    Strong<Dom::Document> _dom;
    Opt<Driver::RenderResult> _renderResult;

    View(Strong<Dom::Document> dom) : _dom(dom) {}

    Style::Media _constructMedia(Math::Vec2i viewport) {
        return {
            .type = MediaType::SCREEN,
            .width = Px{viewport.width},
            .height = Px{viewport.height},
            .aspectRatio = viewport.width / (f64)viewport.height,
            .orientation = Orientation::LANDSCAPE,

            .resolution = Resolution::fromDpi(96),
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::FAST,

            .overflowBlock = OverflowBlock::SCROLL,
            .overflowInline = OverflowInline::SCROLL,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::FINE,
            .hover = Hover::HOVER,
            .anyPointer = Pointer::FINE,
            .anyHover = Hover::HOVER,

            .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
            .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
            .prefersContrast = Contrast::NO_PREFERENCE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = ColorScheme::DARK,
            .prefersReducedData = ReducedData::NO_PREFERENCE,
        };
    }

    void reconcile(View &o) override {
        _dom = o._dom;
        _renderResult = NONE;
    }

    void paint(Gfx::Canvas &g, Math::Recti rect) override {
        auto viewport = bound().size();
        if (not _renderResult) {
            auto media = _constructMedia(viewport);
            _renderResult = Driver::render(*_dom, media, viewport.cast<Px>());
        }

        g.push();

        g.origin(bound().xy.cast<f64>());
        g.clip(viewport);

        auto [layout, paint] = *_renderResult;
        g.clear(rect, Gfx::WHITE);

        paint->paint(g);
        if (Ui::debugShowLayoutBounds) {
            logDebug("layout tree: {}", layout);
            logDebug("paint tree: {}", paint);
            layout->paintWireframe(g);
        }

        g.pop();
    }

    void layout(Math::Recti bound) override {
        _renderResult = NONE;
        Ui::View<View>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i size, Ui::Hint) override {
        // FIXME: This is wasteful, we should cache the result
        auto media = _constructMedia(size);
        auto [layout, _] = Driver::render(*_dom, media, size.cast<Px>());

        logDebug("Size: {}", layout->_box.borderBox);

        return {
            layout->_box.borderBox.width.cast<isize>(),
            layout->_box.borderBox.height.cast<isize>(),
        };
    }
};

Ui::Child view(Strong<Dom::Document> dom) {
    return makeStrong<View>(dom);
}

} // namespace Vaev::View
