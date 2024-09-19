#pragma once

#include <karm-kira/navbar.h>
#include <karm-ui/layout.h>
#include <mdi/alarm.h>
#include <mdi/clock-outline.h>
#include <mdi/dock-bottom.h>
#include <mdi/timer-outline.h>
#include <mdi/timer-sand.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_NAVBAR{
    Mdi::DOCK_BOTTOM,
    "Navigation Bar"s,
    "A horizontal navigation bar that displays a list of links.",
    [] {
        return Ui::state(0, [](auto state, auto bind) {
            return Ui::vflow(
                Ui::grow(NONE),
                Kr::navbarContent({
                    Kr::navbarItem(
                        bind(0),
                        Mdi::ALARM,
                        "Alarm",
                        state == 0
                    ),
                    Kr::navbarItem(
                        bind(1),
                        Mdi::CLOCK_OUTLINE,
                        "Clock",
                        state == 1
                    ),
                    Kr::navbarItem(
                        bind(2),
                        Mdi::TIMER_SAND,
                        "Timer",
                        state == 2
                    ),
                    Kr::navbarItem(
                        bind(3),
                        Mdi::TIMER_OUTLINE,
                        "Stopwatch",
                        state == 3
                    ),
                })
            );
        });
    },
};

} // namespace Hideo::Zoo
