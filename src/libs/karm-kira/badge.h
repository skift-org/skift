#pragma once

#include <karm-ui/view.h>

#include "_prelude.h"

namespace Karm::Kira {

enum struct BadgeStyle {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,

    _LEN
};

Ui::Child badge(Gfx::Color color, String text);

Ui::Child badge(BadgeStyle style, String text);

/// A badge that displays the version of the application based on the build configuration.
Ui::Child versionBadge();

} // namespace Karm::Kira
