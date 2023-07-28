#include "model.h"

namespace Keyboard {

State reduce(State s, Action a) {
    return a.visit(
        Visitor{
            [&](ToggleShift) {
                s.shift = !s.shift;
                return s;
            },
        });
}

} // namespace Keyboard
