#pragma once

#include <karm-ui/reducer.h>

namespace Counter {

struct State {
    bool initial = true;
    isize counter = 0;
};

struct ResetAction {};

struct IncrementAction {};

struct DecrementAction {};

using Action = Var<ResetAction, IncrementAction, DecrementAction>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Counter
