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
};

Ui::Child view(Strong<Dom::Document>) {
    return makeStrong<View>();
}

} // namespace Web::View
