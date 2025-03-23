#include <karm-kira/scaffold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <mdi/console-line.h>

namespace Hideo::Console {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::CONSOLE_LINE,
        .title = "Console"s,
        .body = [] {
            return Ui::codeMedium(" $ ls");
        },
    });
}

} // namespace Hideo::Console

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Console::app());
}
