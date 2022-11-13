#pragma once

#include <karm-ui/react.h>

namespace Counter {

struct State {
    bool initial = true;
    int counter = 0;
};

struct ResetAction {};

struct IncrementAction {};

struct DecrementAction {};

using Actions = Var<ResetAction, IncrementAction, DecrementAction>;

State reduce(State state, Actions action);

using Model = Ui::Model<State, Actions>;

} // namespace Counter
