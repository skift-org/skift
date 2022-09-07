#include "model.h"

namespace Counter {

int reduce(int counter, Actions action) {
    return action.visit(Visitor{
        [&](ResetAction) {
            return 0;
        },
        [&](IncrementAction) {
            return counter + 1;
        },
        [&](DecrementAction) {
            return counter - 1;
        },
    });
}

} // namespace Counter
