#include <karm-ui/view.h>
#include <vaev-driver/render.h>
#include <vaev-layout/paint.h>

#include "view.h"

namespace Vaev::View {

struct View : public Ui::View<View> {
    Gc::Root<Dom::Document> _dom;
    ViewProps _props;
    Opt<Driver::RenderResult> _renderResult;

    View(Gc::Root<Dom::Document> dom, ViewProps props)
        : _dom(dom), _props(props) {}

    Style::Media _constructMedia(Math::Vec2i viewport) {
        return {
            .type = MediaType::SCREEN,
            .width = Au{viewport.width},
            .height = Au{viewport.height},
            .aspectRatio = viewport.width / (f64)viewport.height,
            .orientation = Print::Orientation::LANDSCAPE,

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
            .prefersColorScheme = Ui::darkMode ? ColorScheme::DARK : ColorScheme::LIGHT,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            .deviceWidth = Au{viewport.width},
            .deviceHeight = Au{viewport.height},
            .deviceAspectRatio = viewport.width / (f64)viewport.height,
        };
    }

    void reconcile(View& o) override {
        _dom = o._dom;
        _props = o._props;
        _renderResult = NONE;
    }

    void paint(Gfx::Canvas& g, Math::Recti rect) override {
        auto viewport = bound().size();
        if (not _renderResult) {
            auto media = _constructMedia(viewport);
            _renderResult = Driver::render(*_dom, media, {.small = viewport.cast<Au>()});
        }

        g.push();

        g.origin(bound().xy.cast<f64>());
        g.clip(viewport);

        auto [_, layout, paint, frag] = *_renderResult;
        g.clear(rect, Gfx::WHITE);

        paint->paint(g, rect.offset(-bound().xy).cast<f64>());
        if (_props.wireframe)
            Layout::wireframe(*frag, g);

        g.pop();
    }

    void layout(Math::Recti bound) override {
        _renderResult = NONE;
        Ui::View<View>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i size, Ui::Hint) override {
        // FIXME: This is wasteful, we should cache the result
        auto media = _constructMedia(size);
        auto [_, layout, _, frag] = Driver::render(*_dom, media, {.small = size.cast<Au>()});

        return {
            frag->metrics.borderBox().width.cast<isize>(),
            frag->metrics.borderBox().height.cast<isize>(),
        };
    }
};

Ui::Child view(Gc::Root<Dom::Document> dom, ViewProps props) {
    return makeRc<View>(dom, props);
}

} // namespace Vaev::View
