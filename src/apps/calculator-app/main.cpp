#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/grid.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Calculator {

Ui::Child keypad(State state) {
    Ui::GridStyle gridStyle = {
        .rows = Ui::GridUnit::grow().repeated(7),
        .columns = Ui::GridUnit::grow().repeated(4),
        .gaps = 4,
    };

    return Ui::spacing(
        8,
        Ui::grid(
            gridStyle,

            Ui::cell(
                {0, 0},
                {3, 0},
                Ui::hflow(
                    4,
                    Ui::button(Model::bind<MemClearAction>(), state.hasMem ? Ui::Button::SUBTLE : Ui::Button::SUBTLE_ICON, "MC"),
                    Ui::button(Model::bind<MemRecallAction>(), state.hasMem ? Ui::Button::SUBTLE : Ui::Button::SUBTLE_ICON, "MR"),
                    Ui::button(Model::bind<MemAddAction>(), Ui::Button::SUBTLE, "M+"),
                    Ui::button(Model::bind<MemSubAction>(), Ui::Button::SUBTLE, "M-"),
                    Ui::button(Model::bind<MemStoreAction>(), Ui::Button::SUBTLE, "MS"))),

            Ui::button(Model::bind<ClearAllAction>(), Ui::Button::SECONDARY, 18, "CE"),
            Ui::button(Model::bind<ClearAction>(), Ui::Button::SECONDARY, 18, "C"),
            Ui::button(Model::bind(Operator::TO_PERCENT), Ui::Button::SECONDARY, Media::Icons::PERCENT),
            Ui::button(Model::bind<BackspaceAction>(), Ui::Button::SECONDARY, Media::Icons::BACKSPACE_OUTLINE),

            Ui::button(Ui::NOP, Ui::Button::SECONDARY, 18, "1/x"),
            Ui::button(Model::bind(Operator::SQUARE), Ui::Button::SECONDARY, Media::Icons::FORMAT_SUPERSCRIPT),
            Ui::button(Model::bind(Operator::SQRT), Ui::Button::SECONDARY, Media::Icons::SQUARE_ROOT),
            Ui::button(Model::bind(Operator::DIV), Ui::Button::SECONDARY, Media::Icons::DIVISION),

            Ui::button(Model::bind<Number>(7), Ui::Button::DEFAULT, 18, "7"),
            Ui::button(Model::bind<Number>(8), Ui::Button::DEFAULT, 18, "8"),
            Ui::button(Model::bind<Number>(9), Ui::Button::DEFAULT, 18, "9"),
            Ui::button(Model::bind(Operator::MULT), Ui::Button::SECONDARY, Media::Icons::MULTIPLICATION),

            Ui::button(Model::bind<Number>(4), Ui::Button::DEFAULT, 18, "4"),
            Ui::button(Model::bind<Number>(5), Ui::Button::DEFAULT, 18, "5"),
            Ui::button(Model::bind<Number>(6), Ui::Button::DEFAULT, 18, "6"),
            Ui::button(Model::bind(Operator::SUB), Ui::Button::SECONDARY, Media::Icons::MINUS),

            Ui::button(Model::bind<Number>(1), Ui::Button::DEFAULT, 18, "1"),
            Ui::button(Model::bind<Number>(2), Ui::Button::DEFAULT, 18, "2"),
            Ui::button(Model::bind<Number>(3), Ui::Button::DEFAULT, 18, "3"),
            Ui::button(Model::bind(Operator::ADD), Ui::Button::SECONDARY, Media::Icons::PLUS),

            Ui::button(Model::bind(Operator::INVERT_SIGN), Ui::Button::DEFAULT, 18, "+/-"),
            Ui::button(Model::bind<Number>(0), Ui::Button::DEFAULT, 18, "0"),
            Ui::button(Ui::NOP, Ui::Button::DEFAULT, Media::Icons::CIRCLE_SMALL),
            Ui::button(Model::bind<EqualAction>(), Ui::Button::PRIMARY, Media::Icons::EQUAL)));
}

Ui::Child screen(State state) {
    auto currExpr = Ui::align(
        Layout::Align::VCENTER | Layout::Align::END,
        state.op == Operator::NONE ? Ui::text(16, "") : Ui::text(16, toFmt(state.op), state.lhs));

    auto result = Ui::align(
        Layout::Align::VCENTER | Layout::Align::END,
        Ui::text(32, "{}", state.hasRhs ? state.rhs : state.lhs));

    return Ui::spacing(
        {16, 8}, Ui::vflow(8, currExpr, result));
}

Ui::Child app() {
    return Ui::reducer<Model>({}, reduce, [](auto state) {
        return Ui::maxSize(
            {352, 500},
            Ui::dialogLayer(
                Ui::vflow(
                    Ui::titlebar(
                        Media::Icons::CALCULATOR,
                        "Calculator",
                        Ui::TitlebarStyle::DIALOG),
                    screen(state),
                    Ui::grow(Calculator::keypad(state)))));
    });
}

} // namespace Calculator

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, Calculator::app());
}
