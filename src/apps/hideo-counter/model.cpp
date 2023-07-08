#include "model.h"

namespace Counter {

State reduce(State state, Actions action) {
    return action.visit(Visitor{
        [&](ResetAction) {
            return State{true};
        },
        [&](IncrementAction) {
            return State{
                false,
                state.counter + 1,
            };
        },
        [&](DecrementAction) {
            return State{
                false,
                state.counter - 1,
            };
        },
    });
}

} // namespace Counter
