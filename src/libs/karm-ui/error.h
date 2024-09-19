#pragma once

#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>
#include <mdi/alert-octagon-outline.h>

namespace Karm::Ui {

static inline Child errorScope(Str what, auto callback) {
    Res<Child> child = callback();

    if (child) {
        return child.unwrap();
    }

    return box(
        {
            .padding = 12,
            .borderRadii = 6,
            .borderWidth = 1,
            .borderFill = Gfx::RED700,
            .backgroundFill = Gfx::RED900,
            .foregroundFill = Gfx::RED100,
        },
        hflow(
            6,
            icon(Mdi::ALERT_OCTAGON_OUTLINE, 26) | center(),
            vflow(
                4,
                text(TextStyles::titleMedium().withColor(Gfx::WHITE), "Can't display {}", what),
                text(Io::toSentenceCase(child.none().msg()).unwrap())
            )
        )
    );
}

} // namespace Karm::Ui
