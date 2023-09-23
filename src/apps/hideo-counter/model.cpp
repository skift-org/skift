#include "model.h"

namespace Counter {

void reduce(State &s, Action a) {
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
}

} // namespace Counter
