#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/grid.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Calculator {

Ui::Child keypad() {
    Ui::GridStyle gridStyle = {
        .rows = {
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
        },
        .columns = {
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
            Ui::GridUnit::grow(),
        },
        .gaps = 4,
    };

    return Ui::spacing(
        8,
        Ui::grid(
            gridStyle,

            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::PERCENT),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, 18, "CE"),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, 18, "C"),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::BACKSPACE_OUTLINE),

            Ui::button(Ui::NOP, Ui::Button::SECONDARY, 18, "1/x"),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::FORMAT_SUPERSCRIPT),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::SQUARE_ROOT),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::DIVISION),

            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "7"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "8"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "9"),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::MULTIPLICATION),

            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "4"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "5"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "6"),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::MINUS),

            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "1"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "2"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "3"),
            Ui::button(Ui::NOP, Ui::Button::SECONDARY, Media::Icons::PLUS),

            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "+/-"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, 18, "0"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, Media::Icons::CIRCLE_SMALL),
            Ui::button(Ui::NOP, Ui::Button::PRIMARY, Media::Icons::EQUAL)));
}

Ui::Child screen() {
    return Ui::spacing(
        {16, 8},
        Ui::align(
            Layout::Align::VCENTER | Layout::Align::END,
            Ui::text(24, "0")));
}

Ui::Child app() {
    return Ui::maxSize(
        {320, 420},
        Ui::dialogLayer(
            Ui::vflow(
                Ui::titlebar(
                    Media::Icons::CALCULATOR,
                    "Calculator",
                    Ui::TitlebarStyle::DIALOG),
                screen(),
                Ui::grow(Calculator::keypad()))));
}

} // namespace Calculator

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, Calculator::app());
}
