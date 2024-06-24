#include <karm-ui/view.h>
#include <vaev-base/color.h>

#include "render.h"
#include "view.h"

namespace Vaev::View {

struct View : public Ui::View<View> {
    Strong<Dom::Document> _dom;
    Opt<RenderResult> _renderResult;

    View(Strong<Dom::Document> dom) : _dom(dom) {}

    void paint(Gfx::Context &g, Math::Recti) override {
        if (not _renderResult)
            _renderResult = render(*_dom, bound().size().cast<Px>());

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
