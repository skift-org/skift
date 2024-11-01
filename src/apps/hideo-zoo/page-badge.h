#pragma once

#include <karm-kira/badge.h>
#include <karm-ui/layout.h>
#include <mdi/card.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_BADGE{
    Mdi::CARD,
    "Badge",
    "Displays a badge or a component that looks like a badge.",
    [] {
        return Ui::vflow(
                   16,
                   Math::Align::CENTER,
                   Kr::badge(Kr::BadgeStyle::INFO, "Info"s),
                   Kr::badge(Kr::BadgeStyle::SUCCESS, "Success"s),
                   Kr::badge(Kr::BadgeStyle::WARNING, "Warning"s),
                   Kr::badge(Kr::BadgeStyle::ERROR, "Error"s),
                   Kr::badge(Gfx::GREEN, "New"s)
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
