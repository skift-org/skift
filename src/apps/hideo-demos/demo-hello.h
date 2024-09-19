#pragma once

#include <karm-ui/layout.h>
#include <karm-ui/view.h>
#include <mdi/emoticon-happy-outline.h>
#include <mdi/hand-wave.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo HELLO_DEMO{
    Mdi::HAND_WAVE,
    "Hello, world!",
    "Hello, world!",
    [] {
        return Ui::vflow(
                   16,
                   Math::Align::CENTER,
                   Ui::icon(Mdi::EMOTICON_HAPPY_OUTLINE, 64),
                   Ui::titleLarge("Hello, world!")
               ) |
               Ui::insets(16);
    },
};

} // namespace Hideo::Demos
