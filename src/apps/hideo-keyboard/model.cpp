#include "model.h"

namespace Hideo::Keyboard {

void reduce(State &s, Action a) {
    a.visit(
        Visitor{
            [&](ToggleShift) {
                s.shift = !s.shift;
            },
        }
    );
}

} // namespace Hideo::Keyboard
