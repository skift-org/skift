#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Keyboard {

struct State {
    bool shift = true;
};

struct ToggleShift {
};

using Action = Union<ToggleShift>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Keyboard
