module;

#include <karm-math/align.h>

export module Hideo.Keyboard;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import :model;

namespace Hideo::Keyboard {

static Ui::Child toolbar() {
    return Ui::hflow(
               Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::EMOTICON),
               Ui::empty({128, 4}) |
                   Ui::box({
                       .borderRadii = 999,
                       .backgroundFill = Ui::GRAY50,
                   }) |
                   Ui::center() |
                   Ui::grow(),
               Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::COG_OUTLINE)
           ) |
           Ui::dragRegion();
}

static Ui::Child key(auto icon) {
    return Ui::button(Ui::SINK<>, Ui::titleMedium(icon) | Ui::center() | Ui::pinSize(32));
}

static Ui::Child keyboard(State const& k) {
    auto firstRow = Ui::hflow(
        8,
        key(k.shift ? "Q" : "q"),
        key(k.shift ? "W" : "w"),
        key(k.shift ? "E" : "e"),
        key(k.shift ? "R" : "r"),
        key(k.shift ? "T" : "t"),
        key(k.shift ? "Y" : "y"),
        key(k.shift ? "U" : "u"),
        key(k.shift ? "I" : "i"),
        key(k.shift ? "O" : "o"),
        key(k.shift ? "P" : "p")
    );

    auto secondRow = Ui::hflow(
        8,
        Ui::grow(NONE),
        key(k.shift ? "A" : "a"),
        key(k.shift ? "S" : "s"),
        key(k.shift ? "D" : "d"),
        key(k.shift ? "F" : "f"),
        key(k.shift ? "G" : "g"),
        key(k.shift ? "H" : "h"),
        key(k.shift ? "J" : "j"),
        key(k.shift ? "K" : "k"),
        key(k.shift ? "L" : "l"),
        Ui::grow(NONE)
    );

    auto thirdRow = Ui::hflow(
        8,
        Ui::button(
            Model::bind<ToggleShift>(),
            Ui::ButtonStyle::secondary(),
            k.shift ? Mdi::ARROW_UP_BOLD : Mdi::ARROW_UP_BOLD_OUTLINE
        ) | Ui::grow(),
        key(k.shift ? "Z" : "z"),
        key(k.shift ? "X" : "x"),
        key(k.shift ? "C" : "c"),
        key(k.shift ? "V" : "v"),
        key(k.shift ? "B" : "b"),
        key(k.shift ? "N" : "n"),
        key(k.shift ? "M" : "m"),
        Ui::button(
            Ui::SINK<>,
            Ui::ButtonStyle::secondary(),
            Mdi::BACKSPACE_OUTLINE
        ) | Ui::grow()
    );

    auto fourthRow = Ui::hflow(
                         8,
                         Ui::button(Ui::SINK<>, Ui::ButtonStyle::secondary(), "&123") | Ui::grow(2),
                         key(","),
                         Ui::button(Ui::SINK<>, Ui::empty()) | Ui::grow(6),
                         key("."),
                         Ui::button(Ui::SINK<>, Ui::ButtonStyle::primary(), Mdi::KEYBOARD_RETURN) | Ui::grow(2)
                     ) |
                     Ui::grow();

    return Ui::vflow(
        8,
        firstRow | Ui::hcenterFill() | Ui::grow(),
        secondRow | Ui::grow(),
        thirdRow | Ui::grow(),
        fourthRow | Ui::grow()
    );
}

export Ui::Child flyout() {
    return Ui::reducer<Model>({}, [](auto& k) {
        return Ui::vflow(
                   Kr::separator(),
                   Ui::vflow(
                       8,
                       toolbar(),
                       keyboard(k) | Ui::grow()
                   ) |
                       Ui::hcenterFill() |
                       Ui::minSize({Ui::UNCONSTRAINED, 280}) |
                       Ui::box({
                           .padding = 8,
                           .backgroundFill = Ui::GRAY950,
                       })
               ) |
               Ui::align(Math::Align::HSTRETCH | Math::Align::BOTTOM) |
               Ui::slideIn(Ui::SlideFrom::BOTTOM) |
               Ui::dismisable(
                   Ui::closeDialog,
                   Ui::DismisDir::DOWN,
                   0.15
               );
    });
}

export void show(Ui::Node& n) {
    Ui::showDialog(n, flyout());
}

} // namespace Hideo::Keyboard
