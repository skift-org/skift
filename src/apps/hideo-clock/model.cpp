#include "model.h"

namespace Clock {

State reduce(State state, Actions action) {
    return action.visit(Visitor{
        [&](Page p) {
            state.page = p;
            return state;
        },
    });
}

} // namespace Clock
