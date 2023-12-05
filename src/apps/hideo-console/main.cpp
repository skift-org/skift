#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

namespace Console {

Ui::Child app() {
    return Ui::scafold({
        .icon = Mdi::CONSOLE_LINE,
        .title = "Console",
        .body = Ui::codeMedium(" $ ls"),
        .size = {700, 500},
    });
}

} // namespace Console

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Console::app());
}
