#pragma once

#include <karm-ui/reducer.h>

namespace Counter {

struct ResetAction {};

struct IncrementAction {};

struct DecrementAction {};

using Actions = Var<ResetAction, IncrementAction, DecrementAction>;

int reduce(int model, Actions action);

using Model = Ui::Model<int, Actions>;

} // namespace Counter
