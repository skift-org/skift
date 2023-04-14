#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

namespace MdiGallery {

Ui::Child icons() {
    auto codepoints = Mdi::codepoints();
    auto iconLen = codepoints.len();
    Ui::GridStyle style = Ui::GridStyle::simpleFixed({((isize)iconLen / 8 + 1), 64}, {8, 64});
    return Ui::grid(
        style,
        iter(codepoints)
            .map([](u32 codepoint) {
                return Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Ui::icon((Mdi::Icon)codepoint, 32) | Ui::spacing(10) | Ui::center());
            })
            .collect<Ui::Children>());
}

Ui::Child app() {
    return Ui::vscroll(icons());
}

} // namespace MdiGallery

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, MdiGallery::app() | Ui::maxSize({700, 500}));
}
