#include <karm-ui/box.h>
#include <karm-ui/layout.h>

#include "badge.h"

namespace Karm::Kira {

Ui::Child badge(Gfx::Color color, String t) {
    Ui::BoxStyle boxStyle = {
        .padding = {6, 2},
        .borderRadii = 99,
        .backgroundFill = color.withOpacity(0.2),
        .foregroundFill = color,
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

Ui::Child versionBadge() {
    Ui::Children badges = {};
    badges.pushBack(Kr::badge(
        Kr::BadgeStyle::INFO,
        stringify$(__ck_version_value) ""s
    ));
#ifdef __ck_branch_nightly__
    badges.pushBack(Kr::badge(Gfx::INDIGO400, "Nightly"s));
#elif defined(__ck_branch_stable__)
    // No badge for stable
#else
    badges.pushBack(Kr::badge(Gfx::EMERALD, "Dev"s));
#endif
    return Ui::hflow(4, badges);
}

} // namespace Karm::Kira
