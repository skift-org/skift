#include <karm-sys/context.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>

#include "alert.h"
#include "dialog.h"

namespace Karm::Kira {

Ui::Child alertContent(Ui::Children children) {
    Ui::BoxStyle const boxStyle = {
        .padding = 18,
        .borderRadius = 8,
        .borderWidth = 1,
        .borderPaint = Ui::GRAY800,
        .backgroundPaint = Ui::GRAY900,
        .shadowStyle = Gfx::BoxShadow::elevated(16)
    };

    return Ui::vflow(children) |
           box(boxStyle) |
           Ui::minSize({400, Ui::UNCONSTRAINED}) |
           Ui::dragRegion() |
           Ui::align(Math::Align::CENTER | Math::Align::CLAMP) |
           Ui::spacing(32);
}

Ui::Child alertHeader(Ui::Children children) {
    return Ui::vflow(8, children);
}

Ui::Child alertTitle(String text) {
    return Ui::titleMedium(text);
}

Ui::Child alertDescription(String text) {
    return Ui::bodySmall(Ui::GRAY400, text);
}

Ui::Child alertFooter(Ui::Children children) {
    auto isMobile = Sys::useFormFactor() == Sys::FormFactor::MOBILE;
    return Ui::flow(
               {
                   isMobile ? Math::Flow::TOP_TO_BOTTOM : Math::Flow::LEFT_TO_RIGHT,
                   Math::Align::FILL,
                   8,
               },
               children
           ) |
           Ui::spacing({0, 16, 0, 0});
}

Ui::Child alertAction(Ui::OnPress onPress, String text) {
    return Ui::button(
        [onPress = std::move(onPress)](auto &n) {
            onPress(n);
            Ui::closeDialog(n);
        },
        Ui::ButtonStyle::primary(),
        text
    );
}

Ui::Child alertCancel() {
    return Ui::button(
        Ui::closeDialog,
        "Cancel"
    );
}

Ui::Child alert(String title, String description) {
    return alertContent({
        alertHeader({
            alertTitle(title),
            alertDescription(description),
        }),
        alertFooter({
            Ui::grow(NONE),
            alertAction(Ui::NOP, "Ok"s),
        }),
    });
}

} // namespace Karm::Kira
