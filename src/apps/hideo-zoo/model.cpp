#include "model.h"

namespace Hideo::Zoo {

Ui::Task<Action> reduce(State& s, Action action) {
    action.visit(
        [&](Switch action) {
            s.page = action.page;
        }
    );

    return NONE;
}

} // namespace Hideo::Zoo
