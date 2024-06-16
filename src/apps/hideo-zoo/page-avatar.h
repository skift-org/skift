#pragma once

#include <karm-kira/avatar.h>
#include <karm-ui/layout.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_AVATAR{
    Mdi::CARD,
    "Avatar",
    "An image element with a fallback for representing the user.",
    [] {
        return Ui::vflow(
            16,
            Math::Align::CENTER,
            Kr::avatar("CV"s)
        );
    },
};

} // namespace Hideo::Zoo
