#pragma once

#include <karm-ui/anim.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo CAROUSEL_DEMO{
    Mdi::VIEW_CAROUSEL,
    "Carousel",
    "Carousel",
    [] {
        return Ui::state(
            0,
            [](isize state, auto bind) {
                auto page = [](Gfx::Color color, Str text) {
                    return Ui::box(
                        {
                            .margin = 32,
                            .borderRadii = 8,
                            .borderWidth = 1,
                            .borderFill = Gfx::WHITE.withOpacity(0.1),
                            .backgroundFill = color,
                            .shadowStyle = Gfx::BoxShadow::elevated(24, color.withOpacity(0.5)),
                        },
                        Ui::displayLarge(text) | Ui::center() | Ui::bound()
                    );
                };

                auto prevBtn =
                    Ui::button(
                        bind(clamp(state - 1, 0, 4)),
                        Ui::ButtonStyle::subtle(),
                        Mdi::ARROW_LEFT
                    ) |
                    Ui::insets(8) | Ui::center();

                auto nextBtn =
                    Ui::button(
                        bind(clamp(state + 1, 0, 4)),
                        Ui::ButtonStyle::subtle(),
                        Mdi::ARROW_RIGHT
                    ) |
                    Ui::insets(8) | Ui::center();

                return Ui::stack(
                    Ui::carousel(
                        state,
                        {
                            page(Gfx::RED, "1"),
                            page(Gfx::GREEN, "2"),
                            page(Gfx::BLUE, "3"),
                            page(Gfx::YELLOW, "4"),
                            page(Gfx::CYAN, "5"),
                        }
                    ),

                    Ui::hflow(
                        prevBtn,
                        Ui::grow(NONE),
                        nextBtn
                    )
                );
            }
        );
    },
};

} // namespace Hideo::Demos
