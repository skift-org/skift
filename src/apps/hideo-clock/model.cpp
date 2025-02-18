#include <karm-sys/time.h>

#include "model.h"

namespace Hideo::Clock {

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](Page p) {
            s.page = p;
        },
        [&](TimeTick) {
            s.dateTime = Sys::dateTime();
        }
    });

    return NONE;
}

} // namespace Hideo::Clock
