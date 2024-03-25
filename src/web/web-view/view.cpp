#include <karm-ui/view.h>
#include <web-css/colors.h>

#include "view.h"

namespace Web {

struct View : public Ui::View<View> {
    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clip(bound());
        g.clear(Css::WHITE);
        g.restore();
    }
};

Ui::Child view() {
    return makeStrong<View>();
}

} // namespace Web
