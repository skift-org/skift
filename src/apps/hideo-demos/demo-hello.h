#pragma once

#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Demos {

static inline Demo HELLO_DEMO{
    Mdi::HAND_WAVE,
    "Hello, world!",
    "Hello, world!",
    []() {
        return Ui::vflow(
                   16,
                   Layout::Align::CENTER,
                   Ui::icon(Mdi::Icon::EMOTICON_HAPPY_OUTLINE, 64),
                   Ui::titleLarge("Hello, world!")) |
               Ui::spacing(16);
    },
};

} // namespace Demos
