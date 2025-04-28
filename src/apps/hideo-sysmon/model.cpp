module;

#include <karm-base/union.h>

export module Hideo.Sysmon:model;

import Karm.Ui;

namespace Hideo::Sysmon {

export enum struct Tab {
    APPLICATIONS,
    SERVICES,
    KERNEL,

    PROCESSORS,
    MEMORY,
    DRIVES,
    NETWORK,
    GRAPHICS,
};

export struct State {
    Tab tab = Tab::APPLICATIONS;
};

export struct GoTo {
    Tab tab;
};

export using Action = Union<GoTo>;

Ui::Task<Action> reduce(State& s, Action a) {
    if (auto goTo = a.is<GoTo>())
        s.tab = goTo->tab;
    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Sysmon
