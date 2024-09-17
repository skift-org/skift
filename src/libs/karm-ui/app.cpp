#include "app.h"

#include "_embed.h"

#include "host.h"
#include "input.h"
#include "layout.h"

namespace Karm::Ui {

Child inspector(Child child) {
    return hflow(
        child | Ui::grow(),
        separator(),
        vflow(
            4,
            icon(Mdi::FLASK, 24, GRAY600) | Ui::insets(6) | Ui::center(),
            separator(),
            button(
                [](auto &n) {
                    debugShowLayoutBounds = !debugShowLayoutBounds;
                    Ui::shouldLayout(n);
                },
                ButtonStyle::subtle(),
                Mdi::RULER_SQUARE
            ),
            button(
                [](auto &n) {
                    debugShowRepaintBounds = !debugShowRepaintBounds;
                    Ui::shouldLayout(n);
                },
                ButtonStyle::subtle(),
                Mdi::BRUSH
            ),
            button(
                [](auto &n) {
                    debugShowEmptyBounds = !debugShowEmptyBounds;
                    Ui::shouldLayout(n);
                },
                ButtonStyle::subtle(),
                Mdi::BORDER_NONE_VARIANT
            ),
            button(
                [](auto &n) {
                    debugShowScrollBounds = !debugShowScrollBounds;
                    Ui::shouldLayout(n);
                },
                ButtonStyle::subtle(),
                Mdi::ARROW_UP_DOWN
            ),
            button(
                [](auto &n) {
                    debugShowPerfGraph = !debugShowPerfGraph;
                    Ui::shouldLayout(n);
                },
                ButtonStyle::subtle(),
                Mdi::CHART_HISTOGRAM
            )
        ) |
            Ui::insets(4) |
            Ui::box({
                .backgroundFill = GRAY800,
            })
    );
}

Res<> runApp(Sys::Context &ctx, Child root) {
    auto &args = useArgs(ctx);
    if (args.has("--debug"))
        root = root | inspector;
    return try$(_Embed::makeHost(root))->run();
}

void mountApp(Cli::Command &cmd, Slot rootSlot) {
    Cli::Flag debugArg = Cli::flag(NONE, "debug"s, "Show debug inspector."s);
    Cli::Flag mobileArg = Cli::flag(NONE, "mobile"s, "Show mobile layout."s);

    cmd.option(debugArg);
    cmd.option(mobileArg);
    cmd.callbackAsync = [rootSlot = std::move(rootSlot), debugArg](Sys::Context &) -> Async::Task<> {
        auto root = rootSlot();
        if (debugArg)
            root = root | inspector;
        co_return co_try$(_Embed::makeHost(root))->run();
    };
}

} // namespace Karm::Ui
