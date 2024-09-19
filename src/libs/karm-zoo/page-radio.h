#pragma once

#include <karm-kira/radio.h>
#include <karm-ui/layout.h>
#include <mdi/radiobox-marked.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_RADIO{
    Mdi::RADIOBOX_MARKED,
    "Radio",
    "A set of checkable buttons—known as radio buttons—where no more than one of the buttons can be checked at a time.",
    [] {
        return Kr::radio(
                   true,
                   NONE
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
