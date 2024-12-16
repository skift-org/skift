#pragma once

#include <karm-kira/titlebar.h>
#include <karm-ui/layout.h>
#include <mdi/text-box.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_RICHTEXT{
    Mdi::TEXT_BOX,
    "Rich Text"s,
    "An area that displays text with various styles and formatting options.",
    [] {
        auto prose = makeStrong<Text::Prose>(Ui::TextStyles::bodyMedium());

        prose->append("This is a simple text with no formatting.\n"s);

        prose->append("We can change color: "s);

        prose->pushSpan();
        prose->spanColor(Gfx::RED);
        prose->append("red"s);
        prose->popSpan();
        prose->append(", "s);

        prose->pushSpan();
        prose->spanColor(Gfx::GREEN);
        prose->append(" green"s);
        prose->popSpan();
        prose->append(", "s);

        prose->pushSpan();
        prose->spanColor(Gfx::BLUE);
        prose->append("blue"s);
        prose->popSpan();

        prose->append(".\n"s);

        return Ui::text(prose) | Ui::center();
    },
};

} // namespace Hideo::Zoo
