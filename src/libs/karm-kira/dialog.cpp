#include <karm-sys/context.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>

#include "dialog.h"

namespace Karm::Kira {

Ui::Child dialogContent(Ui::Children children) {
    Ui::BoxStyle const boxStyle = {
        .borderRadii = 8,
        .borderWidth = 1,
        .borderFill = Ui::GRAY800,
        .backgroundFill = Ui::GRAY900,
        .shadowStyle = Gfx::BoxShadow::elevated(16)
    };

    return Ui::vflow(children) |
           box(boxStyle) |
           Ui::dragRegion() |
           Ui::align(Math::Align::CENTER | Math::Align::CLAMP) |
           Ui::insets(32);
}
Ui::Child dialogTitleBar(String title) {
    return Ui::vflow(
        Ui::hflow(
            Ui::titleSmall(title) | Ui::vcenter(),
            Ui::grow(NONE),
            button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::CLOSE)
        ) |
            Ui::insets({16, 4, 4, 4}),
        Ui::separator()
    );
}

Ui::Child dialogHeader(Ui::Children children) {
    return Ui::vflow(8, children) |
           Ui::insets({16, 16, 16, 8});
}

Ui::Child dialogTitle(String text) {
    return Ui::titleMedium(text);
}

Ui::Child dialogDescription(String text) {
    return Ui::bodySmall(Ui::GRAY400, text) |
           Ui::pinSize({380, Ui::UNCONSTRAINED});
}

Ui::Child dialogFooter(Ui::Children children) {
    auto isMobile = Sys::useFormFactor() == Sys::FormFactor::MOBILE;
    if (not isMobile)
        children.pushFront(Ui::grow(NONE));
    return Ui::flow(
               {
                   isMobile ? Math::Flow::TOP_TO_BOTTOM : Math::Flow::LEFT_TO_RIGHT,
                   Math::Align::FILL,
                   8,
               },
               children
           ) |
           Ui::insets({16, 8, 16, 16});
}

Ui::Child dialogAction(Ui::OnPress onPress, String text) {
    return Ui::button(
        [onPress = std::move(onPress)](auto &n) {
            onPress(n);
            Ui::closeDialog(n);
        },
        Ui::ButtonStyle::primary(),
        text
    );
}

Ui::Child dialogCancel() {
    return Ui::button(
        Ui::closeDialog,
        "Cancel"
    );
}

Ui::Child alert(String title, String description) {
    return dialogContent({
        dialogHeader({
            dialogTitle(title),
            dialogDescription(description),
        }),
        dialogFooter({
            dialogAction(Ui::NOP, "Ok"s),
        }),
    });
}

} // namespace Karm::Kira
