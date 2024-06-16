#include <karm-ui/box.h>

#include "badge.h"

namespace Karm::Kira {

Ui::Child badge(Gfx::Color color, String t) {
    Ui::BoxStyle boxStyle = {
        .padding = {6, 2},
        .borderRadius = 99,
        .backgroundPaint = color.withOpacity(0.2),
        .foregroundPaint = color,
    };

    return Ui::labelSmall(t) | box(boxStyle);
}

Ui::Child badge(BadgeStyle style, String t) {
    Array COLORS = {
        Gfx::BLUE400,
        Gfx::LIME400,
        Gfx::YELLOW400,
        Gfx::RED400,
    };

    return badge(COLORS[static_cast<u8>(style)], t);
}

} // namespace Karm::Kira
