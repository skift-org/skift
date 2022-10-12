#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

#include "model.h"

namespace Calculator {

Ui::Child textButton(Ui::OnPress onPress, Ui::ButtonStyle style, String t) {
    return Ui::button(std::move(onPress), style, Ui::bound(Ui::center(Ui::text(Ui::TextStyle::regular().withSize(18), t))));
}

Ui::Child textButton(Ui::OnPress onPress, String t) {
    return textButton(std::move(onPress), Ui::ButtonStyle::regular(), t);
}

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

            textButton(Model::bind<ClearAllAction>(), Ui::ButtonStyle::secondary(), "CE"),
            textButton(Model::bind<ClearAction>(), Ui::ButtonStyle::secondary(), "C"),
            Ui::button(Model::bind(Operator::TO_PERCENT), Ui::ButtonStyle::secondary(), Media::Icons::PERCENT),
            Ui::button(Model::bind<BackspaceAction>(), Ui::ButtonStyle::secondary(), Media::Icons::BACKSPACE_OUTLINE),

            textButton(Model::bind(Operator::RESIPROCAL), Ui::ButtonStyle::secondary(), "1/x"),
            textButton(Model::bind(Operator::SQUARE), Ui::ButtonStyle::secondary(), "x²"),
            textButton(Model::bind(Operator::SQRT), Ui::ButtonStyle::secondary(), "√x"),
            Ui::button(Model::bind(Operator::DIV), Ui::ButtonStyle::secondary(), Media::Icons::DIVISION),

            textButton(Model::bind<Number>(7), "7"),
            textButton(Model::bind<Number>(8), "8"),
            textButton(Model::bind<Number>(9), "9"),
            Ui::button(Model::bind(Operator::MULT), Ui::ButtonStyle::secondary(), Media::Icons::MULTIPLICATION),

            textButton(Model::bind<Number>(4), "4"),
            textButton(Model::bind<Number>(5), "5"),
            textButton(Model::bind<Number>(6), "6"),
            Ui::button(Model::bind(Operator::SUB), Ui::ButtonStyle::secondary(), Media::Icons::MINUS),

            textButton(Model::bind<Number>(1), "1"),
            textButton(Model::bind<Number>(2), "2"),
            textButton(Model::bind<Number>(3), "3"),
            Ui::button(Model::bind(Operator::ADD), Ui::ButtonStyle::secondary(), Media::Icons::PLUS),

            textButton(Model::bind(Operator::INVERT_SIGN), "+/-"),
            textButton(Model::bind<Number>(0), "0"),
            Ui::button(Model::bind<EnterDecimalAction>(), Media::Icons::CIRCLE_SMALL),
            Ui::button(Model::bind<EqualAction>(), Ui::ButtonStyle::primary(), Media::Icons::EQUAL)));
}

Ui::Child screen(State state) {
    // auto debugExpr = Ui::text("op: {}, lhs: {}, rhs: {}", toFmt(state.op), state.lhs, state.rhs);

    auto currExpr = Ui::align(
        Layout::Align::VCENTER | Layout::Align::END,
        state.op == Operator::NONE ? Ui::text(Ui::TextStyle::label(), "") : Ui::text(Ui::TextStyle::label(), toFmt(state.op), state.lhs));

    auto result = Ui::align(
        Layout::Align::VCENTER | Layout::Align::END,
        Ui::text(Ui::TextStyle::label().withSize(32), "{}", state.hasRhs ? state.rhs : state.lhs));

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
