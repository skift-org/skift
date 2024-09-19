#pragma once

#include <karm-kira/titlebar.h>
#include <karm-ui/layout.h>
#include <mdi/dock-top.h>
#include <mdi/duck.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_TITLEBAR{
    Mdi::DOCK_TOP,
    "Titlebar"s,
    "An area at the top of a window that displays the title of the window and may include other elements such as a menu button, close button, and minimize button.",
    [] {
        return Kr::titlebar(Mdi::DUCK, "Cool App"s) | Ui::center();
    },
};

} // namespace Hideo::Zoo
