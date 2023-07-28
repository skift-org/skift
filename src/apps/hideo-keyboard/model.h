#pragma once

#include <karm-ui/reducer.h>

namespace Keyboard {

struct State {
    bool shift = true;
};

struct ToggleShift {
};

using Action = Var<ToggleShift>;

State reduce(State s, Action a);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Keyboard
