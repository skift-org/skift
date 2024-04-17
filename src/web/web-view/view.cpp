#include <karm-ui/view.h>
#include <web-css/colors.h>

#include "view.h"

namespace Web::View {

struct View : public Ui::View<View> {
    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clear(bound(), Css::WHITE);
        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {};
    }
};

Ui::Child view(Strong<Dom::Document>) {
    return makeStrong<View>();
}

} // namespace Web::View
