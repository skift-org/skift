#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Calculator {

Ui::Child textButton(Ui::OnPress onPress, Ui::ButtonStyle style, String t) {
    return Ui::text(Ui::TextStyle::labelLarge().withSize(18), t) |
           Ui::center() |
           Ui::bound() |
           Ui::button(std::move(onPress), style);
}

Ui::Child textButton(Ui::OnPress onPress, String t) {
    return textButton(std::move(onPress), Ui::ButtonStyle::regular(), t);
}

Ui::Child keypad(State const &state) {
    return Ui::spacing(
        8,
        Ui::grid(
            Ui::GridStyle::simpleGrow(7, 4, 4),

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
            Ui::button(Model::bind(Operator::TO_PERCENT), Ui::ButtonStyle::secondary(), Mdi::PERCENT),
            Ui::button(Model::bind<BackspaceAction>(), Ui::ButtonStyle::secondary(), Mdi::BACKSPACE_OUTLINE),

            textButton(Model::bind(Operator::RESIPROCAL), Ui::ButtonStyle::secondary(), "1/x"),
            textButton(Model::bind(Operator::SQUARE), Ui::ButtonStyle::secondary(), "x²"),
            textButton(Model::bind(Operator::SQRT), Ui::ButtonStyle::secondary(), "√x"),
            Ui::button(Model::bind(Operator::DIV), Ui::ButtonStyle::secondary(), Mdi::DIVISION),

            textButton(Model::bind<Number>(7), "7"),
            textButton(Model::bind<Number>(8), "8"),
            textButton(Model::bind<Number>(9), "9"),
            Ui::button(Model::bind(Operator::MULT), Ui::ButtonStyle::secondary(), Mdi::MULTIPLICATION),

            textButton(Model::bind<Number>(4), "4"),
            textButton(Model::bind<Number>(5), "5"),
            textButton(Model::bind<Number>(6), "6"),
            Ui::button(Model::bind(Operator::SUB), Ui::ButtonStyle::secondary(), Mdi::MINUS),

            textButton(Model::bind<Number>(1), "1"),
            textButton(Model::bind<Number>(2), "2"),
            textButton(Model::bind<Number>(3), "3"),
            Ui::button(Model::bind(Operator::ADD), Ui::ButtonStyle::secondary(), Mdi::PLUS),

            textButton(Model::bind(Operator::INVERT_SIGN), "+/-"),
            textButton(Model::bind<Number>(0), "0"),
            Ui::button(Model::bind<EnterDecimalAction>(), Mdi::CIRCLE_SMALL),
            Ui::button(Model::bind<EqualAction>(), Ui::ButtonStyle::primary(), Mdi::EQUAL)));
}

Ui::Child screen(State const &state) {
    // auto debugExpr = Ui::text("op: {}, lhs: {}, rhs: {}", toFmt(state.op), state.lhs, state.rhs);

    auto currExpr =
        (state.op == Operator::NONE ? Ui::text("") : Ui::text(toFmt(state.op), state.lhs)) |
        Ui::align(Layout::Align::VCENTER | Layout::Align::END);

    auto result =
        (state.error ? Ui::headlineMedium(*state.error)
                     : Ui::text(Ui::TextStyle::headlineMedium(), "{}", state.hasRhs ? state.rhs : state.lhs)) |
        Ui::align(Layout::Align::VCENTER | Layout::Align::END);

    return Ui::vflow(8, /* debugExpr, */ currExpr, result) | Ui::spacing({16, 8});
}

Ui::Child app() {
    return Ui::reducer<Model>([](auto const &state) {
        return Ui::scafold({
            .icon = Mdi::CALCULATOR,
            .title = "Calculator",
            .titlebar = Ui::TitlebarStyle::DIALOG,
            .body = Ui::vflow(
                screen(state) | Ui::dragRegion(),
                keypad(state) | Ui::grow()),
            .size = {300, 450},
        });
    });
}

} // namespace Calculator

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Calculator::app());
}
