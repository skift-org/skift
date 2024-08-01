#include <karm-ui/box.h>
#include <karm-ui/layout.h>

#include "avatar.h"

namespace Karm::Kira {

Ui::Child avatar() {
    return avatar(Mdi::ACCOUNT);
}

Ui::Child avatar(String t) {
    Ui::BoxStyle boxStyle = {
        .borderRadii = 99,
        .backgroundFill = Ui::GRAY800,
        .foregroundFill = Ui::GRAY500
    };

    return Ui::labelLarge(t) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

Ui::Child avatar(Mdi::Icon i) {
    Ui::BoxStyle boxStyle = {
        .borderRadii = 99,
        .backgroundFill = Ui::GRAY800,
        .foregroundFill = Ui::GRAY400
    };

    return Ui::icon(i, 26) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

} // namespace Karm::Kira
