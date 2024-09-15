#include <karm-kira/scaffold.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>

#include "model.h"

namespace Hideo::Calculator {

Ui::Child textButton(Ui::OnPress onPress, Ui::ButtonStyle style, String t) {
    return Ui::text(Ui::TextStyles::labelLarge().withSize(18), t) |
           Ui::center() |
           Ui::bound() |
           Ui::button(std::move(onPress), style);
}

Ui::Child textButton(Ui::OnPress onPress, String t) {
    return textButton(std::move(onPress), Ui::ButtonStyle::regular(), t);
}

Ui::Child keypad(State const &state) {
    return Ui::insets(
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
                    Ui::button(Model::bind<MemStoreAction>(), Ui::ButtonStyle::subtle(), "MS")
                )
            ),

            textButton(Model::bind<ClearAllAction>(), Ui::ButtonStyle::secondary(), "CE"s),
            textButton(Model::bind<ClearAction>(), Ui::ButtonStyle::secondary(), "C"s),
            Ui::button(Model::bind(Operator::TO_PERCENT), Ui::ButtonStyle::secondary(), Mdi::PERCENT),
            Ui::button(Model::bind<BackspaceAction>(), Ui::ButtonStyle::secondary(), Mdi::BACKSPACE_OUTLINE),

            textButton(Model::bind(Operator::RESIPROCAL), Ui::ButtonStyle::secondary(), "1/x"s),
            textButton(Model::bind(Operator::SQUARE), Ui::ButtonStyle::secondary(), "x²"s),
            textButton(Model::bind(Operator::SQRT), Ui::ButtonStyle::secondary(), "√x"s),
            Ui::button(Model::bind(Operator::DIV), Ui::ButtonStyle::secondary(), Mdi::DIVISION),

            textButton(Model::bind<Number>(7), "7"s),
            textButton(Model::bind<Number>(8), "8"s),
            textButton(Model::bind<Number>(9), "9"s),
            Ui::button(Model::bind(Operator::MULT), Ui::ButtonStyle::secondary(), Mdi::MULTIPLICATION),

            textButton(Model::bind<Number>(4), "4"s),
            textButton(Model::bind<Number>(5), "5"s),
            textButton(Model::bind<Number>(6), "6"s),
            Ui::button(Model::bind(Operator::SUB), Ui::ButtonStyle::secondary(), Mdi::MINUS),

            textButton(Model::bind<Number>(1), "1"s),
            textButton(Model::bind<Number>(2), "2"s),
            textButton(Model::bind<Number>(3), "3"s),
            Ui::button(Model::bind(Operator::ADD), Ui::ButtonStyle::secondary(), Mdi::PLUS),

            textButton(Model::bind(Operator::INVERT_SIGN), "+/-"s),
            textButton(Model::bind<Number>(0), "0"s),
            Ui::button(Model::bind<EnterDecimalAction>(), Mdi::CIRCLE_SMALL),
            Ui::button(Model::bind<EqualAction>(), Ui::ButtonStyle::primary(), Mdi::EQUAL)
        )
    );
}

Ui::Child screen(State const &state) {
    // auto debugExpr = Ui::text("op: {}, lhs: {}, rhs: {}", toFmt(state.op), state.lhs, state.rhs);

    auto currExpr =
        (state.op == Operator::NONE ? Ui::text("") : Ui::text(toFmt(state.op), state.lhs)) |
        Ui::align(Math::Align::VCENTER | Math::Align::END);

    auto result =
        (state.error ? Ui::headlineMedium(*state.error)
                     : Ui::headlineMedium("{}", state.hasRhs ? state.rhs : state.lhs)) |
        Ui::align(Math::Align::VCENTER | Math::Align::END);

    return Ui::vflow(8, /* debugExpr, */ currExpr, result) | Ui::insets({16, 8});
}

Ui::Child app() {
    return Ui::reducer<Model>([](State const &state) {
        return Kr::scaffold({
            .icon = Mdi::CALCULATOR,
            .title = "Calculator"s,
            .titlebar = Kr::TitlebarStyle::DIALOG,
            .body = slot$(Ui::vflow(
                screen(state) | Ui::dragRegion(),
                keypad(state) | Ui::grow()
            )),
            .size = {300, 332},
        });
    });
}

} // namespace Hideo::Calculator
