#include <karm-ui/box.h>
#include <karm-ui/layout.h>

#include "avatar.h"

namespace Karm::Kr {

Ui::Child avatar(String t) {
    Ui::BoxStyle boxStyle = {
        .borderRadius = 99,
        .backgroundPaint = Ui::GRAY800,
        .foregroundPaint = Ui::GRAY500
    };

    return Ui::labelLarge(t) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

} // namespace Karm::Kr
