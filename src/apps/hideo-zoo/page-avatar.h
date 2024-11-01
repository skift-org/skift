#pragma once

#include <karm-kira/avatar.h>
#include <karm-ui/layout.h>
#include <mdi/account-circle.h>
#include <mdi/cat.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_AVATAR{
    Mdi::ACCOUNT_CIRCLE,
    "Avatar",
    "An image element with a fallback for representing the user.",
    [] {
        return Ui::hflow(
                   16,
                   Kr::avatar(),
                   Kr::avatar("CV"s),
                   Kr::avatar(Mdi::CAT)
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
