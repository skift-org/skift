#pragma once

#include <karm-kira/progress.h>
#include <karm-ui/layout.h>
#include <mdi/loading.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_PROGRESS{
    Mdi::LOADING,
    "Progress",
    "A loading indicator that spins to indicate that the application is busy.",
    [] {
        return Ui::hflow(8, Kr::progress(12), Kr::progress(18), Kr::progress(24), Kr::progress(48)) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
