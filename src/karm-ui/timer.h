#pragma once

#include <karm-time/time.h>

#include "component.h"

namespace Karm::Ui {

struct TimerComponent : public Component<TimerComponent> {
    Time::Timestamp _start;
    Time::Timespan _interval;
};

} // namespace Karm::Ui
