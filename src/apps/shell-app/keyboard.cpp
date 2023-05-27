#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>

#include "app.h"

namespace Shell {

static Ui::Child toolbar() {
    return Ui::hflow(
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::EMOTICON),
               Ui::empty({128, 4}) |
                   Ui::box({
                       .borderRadius = 999,
                       .backgroundPaint = Ui::GRAY50,
                   }) |
                   Ui::center() |
                   Ui::grow(),
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::COG_OUTLINE)) |
           Ui::dragRegion();
}

static Ui::Child key(auto icon) {
    return Ui::button(Ui::NOP, Ui::titleMedium(icon) | Ui::center() | Ui::pinSize(32));
}

static Ui::Child keyboard() {
    auto firstRow = Ui::hflow(
        8,
        key("Q"),
        key("W"),
        key("E"),
        key("R"),
        key("T"),
        key("Y"),
        key("U"),
        key("I"),
        key("O"),
        key("P"));

    auto secondRow = Ui::hflow(
        8,
        Ui::grow(NONE),
        key("A"),
        key("S"),
        key("D"),
        key("F"),
        key("G"),
        key("H"),
        key("J"),
        key("K"),
        key("L"),
        Ui::grow(NONE));

    auto thirdRow = Ui::hflow(
        8,
        Ui::button(Ui::NOP, Ui::ButtonStyle::secondary(), Mdi::APPLE_KEYBOARD_SHIFT) | Ui::grow(),
        key("Z"),
        key("X"),
        key("C"),
        key("V"),
        key("B"),
        key("N"),
        key("M"),
        Ui::button(Ui::NOP, Ui::ButtonStyle::secondary(), Mdi::BACKSPACE_OUTLINE) | Ui::grow());

    auto fourthRow = Ui::hflow(
                         8,
                         Ui::button(Ui::NOP, Ui::ButtonStyle::secondary(), "&123") | Ui::grow(2),
                         key(","),
                         Ui::button(Ui::NOP, Ui::empty()) | Ui::grow(6),
                         key("."),
                         Ui::button(Ui::NOP, Ui::ButtonStyle::primary(), Mdi::KEYBOARD_RETURN) | Ui::grow(2)) |
                     Ui::grow();

    return Ui::vflow(
        8,
        firstRow | Ui::hcenterFill() | Ui::grow(),
        secondRow | Ui::grow(),
        thirdRow | Ui::grow(),
        fourthRow | Ui::grow());
}

Ui::Child keyboardFlyout() {
    return Ui::vflow(
               Ui::grow(NONE),
               Ui::separator(),
               Ui::vflow(
                   8,
                   toolbar(),
                   keyboard() | Ui::grow()) |
                   Ui::hcenterFill() |
                   Ui::minSize({Ui::UNCONSTRAINED, 280}) |
                   Ui::box({
                       .padding = 8,
                       .backgroundPaint = Ui::GRAY950,
                   })) |
           Ui::dismisable(
               Ui::closeDialog,
               Ui::DismisDir::DOWN,
               0.15);
}

} // namespace Shell
