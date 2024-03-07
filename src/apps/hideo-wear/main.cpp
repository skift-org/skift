#include <karm-sys/entry.h>
#include <karm-ui/app.h>

namespace Hideo::Wear {

Ui::Child app() {
    return Ui::text("Hello, world!") |
           Ui::center() |
           Ui::minSize(192);
}

} // namespace Hideo::Wear

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Wear::app());
}
