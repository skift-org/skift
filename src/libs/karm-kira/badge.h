#pragma once

#include <karm-ui/view.h>

namespace Karm::Kr {

enum struct BadgeStyle {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,

    _LEN
};

Ui::Child badge(Gfx::Color color, String text);

Ui::Child badge(BadgeStyle style, String text);

} // namespace Karm::Kr
