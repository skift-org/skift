#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Counter {

struct State {
    bool initial = true;
    isize counter = 0;
};

struct ResetAction {};

struct IncrementAction {};

struct DecrementAction {};

using Action = Union<ResetAction, IncrementAction, DecrementAction>;

Ui::Task<Action> reduce(State&, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Counter
