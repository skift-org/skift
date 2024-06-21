#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>

#include "dialog.h"

namespace Karm::Kira {

Ui::Child dialogContent(Ui::Children children) {
    Ui::BoxStyle const boxStyle = {
        .borderRadius = 8,
        .borderWidth = 1,
        .borderPaint = Ui::GRAY800,
        .backgroundPaint = Ui::GRAY900,
        .shadowStyle = Gfx::BoxShadow::elevated(16)
    };

    return Ui::vflow(children) |
           box(boxStyle) |
           Ui::dragRegion() |
           Ui::align(Math::Align::CENTER | Math::Align::CLAMP) |
           Ui::spacing(16);
}

Ui::Child dialogTitleBar(String title) {
    return Ui::vflow(
        Ui::hflow(
            Ui::titleSmall(title) | Ui::vcenter(),
            Ui::grow(NONE),
            button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::CLOSE)
        ) |
            Ui::spacing({16, 4, 4, 4}),
        Ui::separator()
    );
}

} // namespace Karm::Kira
