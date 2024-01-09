#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

namespace Hideo::Console {

Ui::Child app() {
    return Ui::scafold({
        .icon = Mdi::CONSOLE_LINE,
        .title = "Console",
        .body = Ui::codeMedium(" $ ls"),
    });
}

} // namespace Hideo::Console

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Console::app());
}
