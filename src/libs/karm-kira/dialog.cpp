module;

#include <karm-app/form-factor.h>
#include <karm-app/inputs.h>
#include <karm-gfx/shadow.h>
#include <karm-math/align.h>

export module Karm.Kira:dialog;

import Karm.Ui;
import Mdi;
import :separator;

namespace Karm::Kira {

export Ui::Child dialogContent(Ui::Children children) {
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

export Ui::Child dialogTitleBar(String title) {
    return Ui::vflow(
        Ui::hflow(
            Ui::titleSmall(title) | Ui::vcenter(),
            Ui::grow(NONE),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::CLOSE)
        ) |
            Ui::insets({4, 4, 4, 16}),
        separator()
    );
}

export Ui::Child dialogHeader(Ui::Children children) {
    return Ui::vflow(8, children) |
           Ui::insets({16, 16, 8, 16});
}

export Ui::Child dialogTitle(String text) {
    return Ui::titleMedium(text);
}

export Ui::Child dialogDescription(String text) {
    return Ui::bodySmall(Ui::GRAY400, text) |
           Ui::pinSize({380, Ui::UNCONSTRAINED});
}

export Ui::Child dialogFooter(Ui::Children children) {
    auto isMobile = App::formFactor == App::FormFactor::MOBILE;
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
           Ui::insets({8, 16, 16, 16});
}

export Ui::Child dialogAction(Opt<Ui::Send<>> onPress, String text) {
    return Ui::button(
               [onPress = std::move(onPress)](auto& n) {
                   onPress(n);
                   Ui::closeDialog(n);
               },
               Ui::ButtonStyle::primary(),
               text
           ) |
           Ui::keyboardShortcut(App::Key::ENTER);
}

export Ui::Child dialogCancel() {
    return Ui::button(
        Ui::closeDialog,
        "Cancel"
    );
}

export Ui::Child alertDialog(String title, String description) {
    return dialogContent({
        dialogHeader({
            dialogTitle(title),
            dialogDescription(description),
        }),
        dialogFooter({
            dialogAction(Ui::SINK<>, "Ok"s),
        }),
    });
}

} // namespace Karm::Kira
