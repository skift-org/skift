#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Sysmon {

enum struct Tab {
    APPLICATIONS,
    SERVICES,
    KERNEL,

    PROCESSORS,
    MEMORY,
    DRIVES,
    NETWORK,
    GRAPHICS,
};

struct State {
    Tab tab = Tab::APPLICATIONS;
};

struct GoTo {
    Tab tab;
};

using Action = Union<GoTo>;

Ui::Task<Action> reduce(State& s, Action a);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Sysmon
