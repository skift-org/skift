#include "model.h"

namespace Hideo::Zoo {

void reduce(State &s, Action action) {
    action.visit(
        [&](Switch action) {
            s.page = action.page;
        }
    );
}

} // namespace Hideo::Zoo
