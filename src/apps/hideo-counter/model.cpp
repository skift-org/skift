module;

#include <karm-base/union.h>

export module Hideo.Counter:model;

import Karm.Ui;

namespace Hideo::Counter {

export struct State {
    bool initial = true;
    isize counter = 0;
};

export struct ResetAction {};

export struct IncrementAction {};

export struct DecrementAction {};

export using Action = Union<
    ResetAction,
    IncrementAction,
    DecrementAction>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](ResetAction) {
            s = State{.initial = true};
        },
        [&](IncrementAction) {
            s.initial = false;
            s.counter += 1;
        },
        [&](DecrementAction) {
            s.initial = false;
            s.counter -= 1;
        },
    });

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Counter
