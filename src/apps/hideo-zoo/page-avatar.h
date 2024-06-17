#pragma once

#include <karm-kira/avatar.h>
#include <karm-ui/layout.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_AVATAR{
    Mdi::ACCOUNT_CIRCLE,
    "Avatar",
    "An image element with a fallback for representing the user.",
    [] {
        return Kr::avatar("CV"s) | Ui::center();
    },
};

} // namespace Hideo::Zoo
