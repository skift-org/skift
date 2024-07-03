#include <karm-ui/view.h>
#include <vaev-base/color.h>

#include "render.h"
#include "view.h"

namespace Vaev::View {

struct View : public Ui::View<View> {
    Strong<Dom::Document> _dom;
    Opt<RenderResult> _renderResult;

    View(Strong<Dom::Document> dom) : _dom(dom) {}

    Style::Media _constructMedia() {
        return {
            .type = MediaType::SCREEN,
            .width = Px{bound().width},
            .height = Px{bound().height},
            .aspectRatio = bound().width / (f64)bound().height,
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

    void paint(Gfx::Context &g, Math::Recti) override {
        if (not _renderResult) {
            auto media = _constructMedia();
            _renderResult = render(*_dom, media, bound().size().cast<Px>());
        }

        g.save();

        g.origin(bound().xy);
        g.clip(bound().size());
        g.clear(bound().size(), WHITE);

        auto [layout, paint] = *_renderResult;

        paint->paint(g);
        if (Ui::debugShowLayoutBounds)
            layout->debug(g);

        g.restore();
    }

    void layout(Math::Recti bound) override {
        _renderResult = NONE;
        Ui::View<View>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {};
    }
};

Ui::Child view(Strong<Dom::Document> dom) {
    return makeStrong<View>(dom);
}

} // namespace Vaev::View
