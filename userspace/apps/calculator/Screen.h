#pragma once

#include <libwidget/Components.h>
#include <libwidget/Layouts.h>

#include "calculator/Engine.h"

namespace Calculator
{

auto screen(Engine &calculator)
{
    using namespace Widget;

    // clang-format off

    return observer(calculator, [&] {
        return panel(
            fill(
                spacing(
                    16,
                    label(IO::format("{}", calculator.screen()), Math::Anchor::CENTER)
                )
            )
        );
    });

    // clang-format on
}

} // namespace Calculator
