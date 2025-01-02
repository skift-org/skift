#include <mdi/arrow-up-down.h>
#include <mdi/border-none-variant.h>
#include <mdi/brush.h>
#include <mdi/chart-histogram.h>
#include <mdi/flask.h>
#include <mdi/ruler-square.h>

#include "_embed.h"
#include "app.h"
#include "host.h"

namespace Karm::Ui {

Res<> runApp(Sys::Context &, Child root) {
    return try$(_Embed::makeHost(root))->run();
}

void mountApp(Cli::Command &cmd, Slot rootSlot) {
    Cli::Flag mobileArg = Cli::flag(NONE, "mobile"s, "Show mobile layout."s);

    cmd.option(mobileArg);
    cmd.callbackAsync = [rootSlot = std::move(rootSlot)](Sys::Context &) -> Async::Task<> {
        auto root = rootSlot();
        co_return co_try$(_Embed::makeHost(root))->run();
    };
}

} // namespace Karm::Ui
