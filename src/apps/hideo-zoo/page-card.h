#pragma once

#include <karm-kira/card.h>
#include <karm-ui/layout.h>
#include <mdi/rectangle.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_CARD{
    Mdi::RECTANGLE,
    "Card",
    "A container that groups and styles its children.",
    [] {
        return Ui::labelMedium(
                   Ui::GRAY700,
                   "This is a card"
               ) |
               Ui::center() |
               Ui::pinSize({400, 300}) |
               Kr::card() | Ui::center();
    },
};

} // namespace Hideo::Zoo
