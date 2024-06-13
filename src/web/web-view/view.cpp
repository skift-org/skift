#include <karm-ui/view.h>
#include <web-base/color.h>

#include "view.h"

namespace Web::View {

struct View : public Ui::View<View> {
    Strong<Dom::Document> _dom;

    View(Strong<Dom::Document> dom) : _dom(dom) {}

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clear(bound(), WHITE);
        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {};
    }
};

Ui::Child view(Strong<Dom::Document> dom) {
    return makeStrong<View>(dom);
}

} // namespace Web::View
