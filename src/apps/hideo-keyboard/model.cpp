#include "model.h"

namespace Hideo::Keyboard {

Ui::Task<Action> reduce(State &s, Action a) {
    a.visit(
        Visitor{
            [&](ToggleShift) {
                s.shift = !s.shift;
            },
        }
    );

    return NONE;
}

} // namespace Hideo::Keyboard
