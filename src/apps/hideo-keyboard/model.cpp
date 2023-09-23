#include "model.h"

namespace Keyboard {

void reduce(State &s, Action a) {
    a.visit(
        Visitor{
            [&](ToggleShift) {
                s.shift = !s.shift;
            },
        });
}

} // namespace Keyboard
