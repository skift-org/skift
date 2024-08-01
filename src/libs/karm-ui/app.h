#pragma once

#include <karm-sys/context.h>

#include "_embed.h"

#include "host.h"
#include "input.h"
#include "layout.h"
#include "reducer.h"

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

inline Res<> runApp(Sys::Context &ctx, Child root) {
    auto &args = useArgs(ctx);
    if (args.has("--debug")) {
        root = inspector(root);
    }
    auto host = try$(_Embed::makeHost(root));
    return host->run();
}

} // namespace Karm::Ui
