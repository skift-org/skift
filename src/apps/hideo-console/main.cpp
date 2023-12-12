#include <karm-main/main.h>
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

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Console::app());
}
