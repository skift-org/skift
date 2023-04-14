#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

namespace Hello {

Ui::Child app() {
    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               Ui::icon(Mdi::Icon::EMOTICON_HAPPY_OUTLINE, 64),
               Ui::titleLarge("Hello, world!")) |
           Ui::spacing(16);
}

} // namespace Hello

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hello::app());
}
