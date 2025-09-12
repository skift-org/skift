module;

#include <karm-sys/time.h>

export module Hideo.Clock:model;

import Karm.Ui;

using namespace Karm;

namespace Hideo::Clock {

export enum struct Page {
    ALARM,
    CLOCK,
    TIMER,
    STOPWATCH,

    _DEFAULT = ALARM,
};

export Str toStr(Page page) {
    switch (page) {
    case Page::CLOCK:
        return "Clock";
    case Page::STOPWATCH:
        return "Stopwatch";
    case Page::TIMER:
        return "Timer";
    case Page::ALARM:
        return "Alarm";
    default:
        return "Unknown";
    }
}

export struct State {
    DateTime dateTime;
    Page page = Page::_DEFAULT;
};

export struct TimeTick {};

using Action = Union<Page, TimeTick>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{[&](Page p) {
                        s.page = p;
                    },
                    [&](TimeTick) {
                        s.dateTime = Sys::dateTime();
                    }});

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Clock
