#pragma once

#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo ICONS_DEMO{
    Mdi::COFFEE,
    "Icons",
    "Icons rendering",
    [] {
        auto codepoints = Mdi::codepoints();
        auto iconLen = codepoints.len();
        Ui::GridStyle style = Ui::GridStyle::simpleFixed({((isize)iconLen / 8 + 1), 64}, {8, 64});
        return Ui::grid(
                   style,
                   iter(codepoints)
                       .map([](u32 codepoint) {
                           return Ui::button(
                               Ui::NOP,
                               Ui::ButtonStyle::subtle(),
                               Ui::icon((Mdi::Icon)codepoint, 32) | Ui::insets(10) | Ui::center()
                           );
                       })
                       .collect<Ui::Children>()
               ) |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
