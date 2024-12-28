#include "model.h"

namespace Hideo::Counter {

Ui::Task<Action> reduce(State &s, Action a) {
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

} // namespace Hideo::Counter
