#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

#include "model.h"

namespace Calculator {

Ui::Child keypad(State state) {
    return Ui::spacing(
        8,
        Ui::grid(
            Ui::GridStyle::simple(7, 4, 4),

            Ui::cell(
                {0, 0},
                {3, 0},
                Ui::hflow(
                    4,
                    Ui::button(Model::bindIf<MemClearAction>(state.hasMem), Ui::ButtonStyle::subtle(), "MC"),
                    Ui::button(Model::bindIf<MemRecallAction>(state.hasMem), Ui::ButtonStyle::subtle(), "MR"),
                    Ui::button(Model::bind<MemAddAction>(), Ui::ButtonStyle::subtle(), "M+"),
                    Ui::button(Model::bind<MemSubAction>(), Ui::ButtonStyle::subtle(), "M-"),
                    Ui::button(Model::bind<MemStoreAction>(), Ui::ButtonStyle::subtle(), "MS"))),

            Ui::button(Model::bind<ClearAllAction>(), Ui::ButtonStyle::secondary(), 18, "CE"),
            Ui::button(Model::bind<ClearAction>(), Ui::ButtonStyle::secondary(), 18, "C"),
            Ui::button(Model::bind(Operator::TO_PERCENT), Ui::ButtonStyle::secondary(), Media::Icons::PERCENT),
            Ui::button(Model::bind<BackspaceAction>(), Ui::ButtonStyle::secondary(), Media::Icons::BACKSPACE_OUTLINE),

            Ui::button(NONE, Ui::ButtonStyle::secondary(), 18, "1/x"),
            Ui::button(Model::bind(Operator::SQUARE), Ui::ButtonStyle::secondary(), 18, "x²"),
            Ui::button(Model::bind(Operator::SQRT), Ui::ButtonStyle::secondary(), 18, "√x"),
            Ui::button(Model::bind(Operator::DIV), Ui::ButtonStyle::secondary(), Media::Icons::DIVISION),

            Ui::button(Model::bind<Number>(7), 18, "7"),
            Ui::button(Model::bind<Number>(8), 18, "8"),
            Ui::button(Model::bind<Number>(9), 18, "9"),
            Ui::button(Model::bind(Operator::MULT), Ui::ButtonStyle::secondary(), Media::Icons::MULTIPLICATION),

            Ui::button(Model::bind<Number>(4), 18, "4"),
            Ui::button(Model::bind<Number>(5), 18, "5"),
            Ui::button(Model::bind<Number>(6), 18, "6"),
            Ui::button(Model::bind(Operator::SUB), Ui::ButtonStyle::secondary(), Media::Icons::MINUS),

            Ui::button(Model::bind<Number>(1), 18, "1"),
            Ui::button(Model::bind<Number>(2), 18, "2"),
            Ui::button(Model::bind<Number>(3), 18, "3"),
            Ui::button(Model::bind(Operator::ADD), Ui::ButtonStyle::secondary(), Media::Icons::PLUS),

            Ui::button(Model::bind(Operator::INVERT_SIGN), 18, "+/-"),
            Ui::button(Model::bind<Number>(0), 18, "0"),
            Ui::button(NONE, Media::Icons::CIRCLE_SMALL),
            Ui::button(Model::bind<EqualAction>(), Ui::ButtonStyle::primary(), Media::Icons::EQUAL)));
}

Ui::Child screen(State state) {
    // auto debugExpr = Ui::text("op: {}, lhs: {}, rhs: {}", toFmt(state.op), state.lhs, state.rhs);

    auto currExpr = Ui::align(
        Layout::Align::VCENTER | Layout::Align::END,
        state.op == Operator::NONE ? Ui::text(16, "") : Ui::text(16, toFmt(state.op), state.lhs));

    auto result = Ui::align(
        Layout::Align::VCENTER | Layout::Align::END,
        Ui::text(32, "{}", state.hasRhs ? state.rhs : state.lhs));

    return Ui::spacing(
        {16, 8}, Ui::vflow(8, /* debugExpr, */ currExpr, result));
}

Ui::Child app() {
    return Ui::reducer<Model>({}, reduce, [](auto state) {
        return Ui::maxSize(
            {300, 450},
            Ui::dialogLayer(
                Ui::vflow(
                    Ui::titlebar(
                        Media::Icons::CALCULATOR,
                        "Calculator",
                        Ui::TitlebarStyle::DIALOG),
                    Ui::dragRegion(screen(state)),
                    Ui::grow(Calculator::keypad(state)))));
    });
}

} // namespace Calculator

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, Calculator::app());
}
